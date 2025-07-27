#include "ui.h"
#include <stdint.h>
#include <stddef.h>
#include "libc/string.h"
#include "libc/stdio.h"
#include "graphics.h"
#include "framebuffer.h"
#include "font.h"
#include "mouse.h"
#include "keyboard.h"
#include "desktop.h"
#include "mm/mm.h"
#include "process.h"

// Screen dimensions and framebuffer are now in framebuffer.h

// Forward declarations for internal functions
static void draw_rect(int x, int y, int width, int height, uint32_t color);
static void draw_rounded_rect(int x, int y, int width, int height, int radius, uint32_t color);
static void draw_string(const char *str, int x, int y, uint32_t color);

// Global UI state
ui_state_t ui_state;

// Window management
static ui_window_t windows[MAX_WINDOWS];
static int window_count = 0;
static int active_window = -1;
static int dragging_window = -1;

// Taskbar state
typedef struct {
    char name[32];
    void (*on_click)(void);
    uint32_t x, width;
} taskbar_icon_t;

#define MAX_TASKBAR_ICONS 10
static taskbar_icon_t taskbar_icons[MAX_TASKBAR_ICONS];
static uint8_t num_taskbar_icons = 0;

// Check if a point is inside a rectangle
static int point_in_rect(int x, int y, int rect_x, int rect_y, int width, int height) {
    return (x >= rect_x && x < rect_x + width && 
            y >= rect_y && y < rect_y + height);
}

// Create a new window
int ui_create_window(int x, int y, int width, int height, const char *title, uint32_t color) {
    if (window_count >= MAX_WINDOWS) return -1;
    
    ui_window_t *win = &windows[window_count];
    win->x = x;
    win->y = y;
    win->width = width;
    win->height = height;
    strncpy(win->title, title, sizeof(win->title) - 1);
    win->title[sizeof(win->title) - 1] = '\0';
    win->color = color;
    win->visible = 1;
    win->active = 0;
    win->dragging = 0;
    win->minimized = 0;
    win->maximized = 0;
    
    return window_count++; // Return window ID and increment for next window
}

// Initialize UI system
void ui_init(uint32_t *fb, uint32_t width, uint32_t height) {
    // Initialize graphics and framebuffer
    init_graphics(fb, width, height);
    
    // Initialize UI state
    memset(&ui_state, 0, sizeof(ui_state));
    ui_state.framebuffer = framebuffer; // Use the global framebuffer
    ui_state.width = fb_width;
    ui_state.height = fb_height;
    ui_state.taskbar_visible = 1;
    ui_state.taskbar_height = TASKBAR_HEIGHT;
    
    // Initialize windows array
    memset(windows, 0, sizeof(windows));
    window_count = 0;
    active_window = -1;
    dragging_window = -1;
    
    // Initialize taskbar icons
    memset(taskbar_icons, 0, sizeof(taskbar_icons));
    num_taskbar_icons = 0;
    
    // Initialize taskbar
    taskbar_init();
    
    // Add some default windows
    ui_create_window(100, 100, 400, 300, "Terminal", UI_COLOR_PRIMARY);
    ui_create_window(200, 150, 500, 400, "File Manager", UI_COLOR_SECONDARY);
    
    printf("UI system initialized with %dx%d resolution\n", fb_width, fb_height);
}

// Draw a rectangle
static void draw_rect(int x, int y, int width, int height, uint32_t color) {
    fb_draw_rect(x, y, width, height, color);
}

// Draw a rectangle with rounded corners
static void draw_rounded_rect(int x, int y, int width, int height, int radius, uint32_t color) {
    (void)radius; // Suppress unused parameter warning
    // For now, just draw a regular rectangle
    fb_draw_rect(x, y, width, height, color);
}

// Taskbar implementation
void taskbar_init(void) {
    num_taskbar_icons = 0;
    // These functions should be implemented elsewhere
    taskbar_add_icon("Start", NULL);
    taskbar_add_icon("Files", NULL);
    taskbar_add_icon("Terminal", NULL);
}

void taskbar_draw(void) {
    // Draw taskbar background
    for (uint32_t y = ui_state.height - TASKBAR_HEIGHT; y < ui_state.height; y++) {
        for (uint32_t x = 0; x < ui_state.width; x++) {
            ui_state.framebuffer[y * ui_state.width + x] = TASKBAR_COLOR;
        }
    }
    
    // Draw taskbar icons
    for (uint8_t i = 0; i < num_taskbar_icons; i++) {
        uint32_t icon_x = 10 + (i * 100);
        uint32_t icon_width = 90;
        
        taskbar_icons[i].x = icon_x;
        taskbar_icons[i].width = icon_width;
        
        ui_draw_string(taskbar_icons[i].name, icon_x + 10, 
                      ui_state.height - TASKBAR_HEIGHT + 12, TASKBAR_TEXT_COLOR);
    }
    
    // Draw system tray with current time (placeholder)
    char time_str[16];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", 12, 34);
    uint32_t time_width = strlen(time_str) * 8;
    ui_draw_string(time_str, ui_state.width - time_width - 20, 
                  ui_state.height - TASKBAR_HEIGHT + 12, TASKBAR_TEXT_COLOR);
}

void taskbar_add_icon(const char *name, void (*on_click)(void)) {
    if (num_taskbar_icons >= MAX_TASKBAR_ICONS) return;
    
    strncpy(taskbar_icons[num_taskbar_icons].name, name, 31);
    taskbar_icons[num_taskbar_icons].name[31] = '\0';
    taskbar_icons[num_taskbar_icons].on_click = on_click;
    num_taskbar_icons++;
}

void taskbar_handle_click(int x, int y) {
    if (y < (int)(fb_height - ui_state.taskbar_height)) return;
    
    for (uint8_t i = 0; i < num_taskbar_icons; i++) {
        if (x >= taskbar_icons[i].x && x <= taskbar_icons[i].x + taskbar_icons[i].width) {
            if (taskbar_icons[i].on_click) {
                taskbar_icons[i].on_click();
            }
            break;
        }
    }
}

// Window management
void wm_add_window(ui_window_t *window) {
    if (window_count >= MAX_WINDOWS) return;
    
    // Add window to the windows array
    windows[window_count] = *window;
    window_count++;
    
    // Set as active window
    if (active_window == -1) {
        active_window = window_count - 1;
    }
}

void wm_draw_all(void) {
    // Draw all windows in reverse order (back to front)
    for (int i = window_count - 1; i >= 0; i--) {
        ui_window_t *win = &windows[i];
        if (!win->visible || win->minimized) continue;
        
        // Draw window background
        for (int y = 0; y < win->height; y++) {
            for (int x = 0; x < win->width; x++) {
                int px = win->x + x;
                int py = win->y + y;
                if (px >= 0 && py >= 0 && px < fb_width && py < fb_height) {
                    framebuffer[py * fb_width + px] = win->bg_color;
                }
            }
        }
        
        // Draw title bar (simplified for now)
        int title_bar_height = 24;
        for (int x = 1; x < win->width - 1; x++) {
            for (int y = 1; y < title_bar_height; y++) {
                int px = win->x + x;
                int py = win->y + y;
                if (px > 0 && py > 0 && px < fb_width - 1 && py < fb_height) {
                    framebuffer[py * fb_width + px] = 
                        (i == active_window) ? 0x3366CC : 0x888888; // Active/Inactive color
                }
            }
        }
        
        // Draw window title
        if (win->title[0] != '\0') {
            draw_string(win->title, win->x + 8, win->y + 6, 0xFFFFFF);
        }
    }
}

// Draw a window
static void draw_window(ui_window_t *window) {
    if (!window->visible) return;
    
    // Draw window border
    draw_rect(window->x, window->y, window->width, window->height, window->border_color);
    
    // Draw title bar
    int title_bar_height = 24;
    draw_rect(window->x + 1, window->y + 1, 
             window->width - 2, title_bar_height, 
             window->title_bar_color);
    
    // Draw title text
    draw_string(window->title, window->x + 8, window->y + 6, window->title_text_color);
    
    // Draw window content area
    draw_rect(window->x + 2, window->y + title_bar_height + 1, 
             window->width - 4, window->height - title_bar_height - 3, 
             window->bg_color);
    
    // Draw window controls (minimize, maximize, close)
    int button_size = 16;
    int button_padding = 4;
    int button_y = window->y + (title_bar_height - button_size) / 2;
    int button_x = window->x + window->width - button_padding - button_size;
    
    // Close button
    draw_rect(button_x, button_y, button_size, button_size, 0xFF4444);
    button_x -= (button_size + button_padding);
    
    // Maximize button
    draw_rect(button_x, button_y, button_size, button_size, 0x44FF44);
    button_x -= (button_size + button_padding);
    
    // Minimize button
    draw_rect(button_x, button_y, button_size, button_size, 0x4444FF);
}

// Draw the taskbar
static void draw_taskbar(void) {
    int taskbar_y = fb_height - ui_state.taskbar_height;
    
    // Draw taskbar background
    draw_rect(0, taskbar_y, fb_width, ui_state.taskbar_height, TASKBAR_COLOR);
    
    // Draw start button
    draw_rounded_rect(5, taskbar_y + 5, 80, ui_state.taskbar_height - 10, 5, 0x1E88E5);
    draw_string("Start", 30, taskbar_y + (ui_state.taskbar_height - 8) / 2, 0xFFFFFF);
    
    // Draw taskbar items (open windows)
    int item_x = 90;
    for (int i = 0; i < window_count; i++) {
        if (!windows[i].visible) continue;
        
        int is_active = (i == window_count - 1); // Top window is active
        uint32_t bg_color = is_active ? 0x4CAF50 : 0x3F51B5;
        
        draw_rounded_rect(item_x, taskbar_y + 5, 
                         TASKBAR_ITEM_WIDTH, ui_state.taskbar_height - 10, 
                         5, bg_color);
        
        // Truncate title if too long
        char title[20];
        strncpy(title, windows[i].title, sizeof(title) - 1);
        title[sizeof(title) - 1] = '\0';
        if (strlen(windows[i].title) > 15) {
            strcpy(title + 12, "...");
        }
        
        draw_string(title, item_x + 8, 
                   taskbar_y + (ui_state.taskbar_height - 8) / 2, 
                   0xFFFFFF);
        
        item_x += TASKBAR_ITEM_WIDTH + 5;
    }
    
    // Draw system tray (right side of taskbar)
    char time_str[32];
    // In a real OS, get actual time here
    strcpy(time_str, "12:00 PM");
    
    int time_width = strlen(time_str) * 8;
    draw_string(time_str, fb_width - time_width - 20, 
               taskbar_y + (ui_state.taskbar_height - 8) / 2, 
               0xFFFFFF);
}

// Handle taskbar click events
static void handle_taskbar_click(int x, int y) {
    int taskbar_y = fb_height - ui_state.taskbar_height;
    
    // Handle start button click
    if (x >= 5 && x < 85 && y >= taskbar_y + 5 && y < taskbar_y + ui_state.taskbar_height - 5) {
        // Toggle start menu (not implemented)
        return;
    }
    
    // Handle window taskbar items
    int item_x = 90;
    for (int i = 0; i < window_count; i++) {
        if (!windows[i].visible) continue;
        
        if (x >= item_x && x < item_x + TASKBAR_ITEM_WIDTH &&
            y >= taskbar_y + 5 && y < taskbar_y + ui_state.taskbar_height - 5) {
            
            // Bring window to front
            if (i != window_count - 1) {
                ui_window_t win = windows[i];
                memmove(&windows[i], &windows[i + 1], 
                       (window_count - i - 1) * sizeof(ui_window_t));
                windows[window_count - 1] = win;
            }
            
            // Toggle window visibility if already active
            if (i == window_count - 1) {
                windows[i].visible = !windows[i].visible;
            }
            
            break;
        }
        
        item_x += TASKBAR_ITEM_WIDTH + 5;
    }
}

// Handle window click events
static void handle_window_click(ui_window_t *window, int x, int y, int buttons) {
    (void)buttons; // Unused parameter
    int title_bar_height = 24;
    int button_size = 16;
    int button_padding = 4;
    int close_x = window->x + window->width - button_padding - button_size;
    
    // Check if close button was clicked
    if (x >= close_x && x < close_x + button_size &&
        y >= window->y + (title_bar_height - button_size) / 2 && 
        y < window->y + (title_bar_height + button_size) / 2) {
        window->visible = 0; // Hide window
        return;
    }
    
    // TODO: Handle window dragging, resizing, etc.
}

// Update the UI
void ui_update(void) {
    // Clear the screen with desktop background
    for (int y = 0; y < fb_height - (ui_state.taskbar_visible ? ui_state.taskbar_height : 0); y++) {
        for (int x = 0; x < fb_width; x++) {
            framebuffer[y * fb_width + x] = 0x1E88E5; // Light blue desktop background
        }
    }
    
    // Draw all windows
    wm_draw_all();
    
    // Draw taskbar if visible
    if (ui_state.taskbar_visible) {
        draw_taskbar();
    }
}

// Handle mouse input
void ui_handle_click(int x, int y, int buttons) {
    // Check if click was on taskbar
    if (y >= (int)(fb_height - ui_state.taskbar_height)) {
        handle_taskbar_click(x, y);
        return;
    }
    
    // Check if click was on a window
    for (int i = window_count - 1; i >= 0; i--) {
        ui_window_t *win = &windows[i];
        if (win->visible && !win->minimized &&
            x >= win->x && x < win->x + (int)win->width &&
            y >= win->y && y < win->y + (int)win->height) {
            
            // Bring window to front
            if (i != window_count - 1) {
                ui_window_t temp = windows[i];
                for (int j = i; j < window_count - 1; j++) {
                    windows[j] = windows[j + 1];
                }
                windows[window_count - 1] = temp;
                active_window = window_count - 1;
            }
            
            // Handle window click
            handle_window_click(win, x, y, buttons);
            return;
        }
    }
}

// Handle mouse movement (for hover effects, window dragging, etc.)
void ui_handle_mouse_move(int x, int y, int buttons) {
    // Handle window dragging
    if (dragging_window != -1 && (buttons & 0x1)) {
        ui_window_t *win = &windows[dragging_window];
        win->x = x - win->drag_start_x;
        win->y = y - win->drag_start_y;
        
        // Keep window within screen bounds
        if (win->x < 0) win->x = 0;
        if (win->y < 0) win->y = 0;
        if (win->x + win->width > (int)fb_width) win->x = fb_width - win->width;
        if (win->y + win->height > (int)fb_height - ui_state.taskbar_height) {
            win->y = fb_height - ui_state.taskbar_height - win->height;
        }
    }
}

// Draw a string to the screen
static void draw_string(const char *str, int x, int y, uint32_t color) {
    if (str) {
        fb_draw_string(str, x, y, color);
    }
}

// Public function to draw a string
void ui_draw_string(const char *str, int x, int y, uint32_t color) {
    draw_string(str, x, y, color);
}
