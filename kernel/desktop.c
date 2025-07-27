#include <stdint.h>
#include <stdio.h>
#include "desktop.h"
#include "window.h"
#include "mouse.h"
#include "keyboard.h"
#include "text.h"
#include <string.h>

#define MAX_NOTEPADS 4
#define NOTEPAD_BUF_SIZE 256

typedef struct {
    int window_id;
    char filename[32];
    char buffer[NOTEPAD_BUF_SIZE];
    int len;
} notepad_t;

notepad_t notepads[MAX_NOTEPADS];
int notepad_count = 0;

static notepad_t* find_notepad(int window_id) {
    for (int i = 0; i < notepad_count; i++) {
        if (notepads[i].window_id == window_id) return &notepads[i];
    }
    return NULL;
}

static notepad_t* create_notepad(int window_id, const char* filename) {
    if (notepad_count >= MAX_NOTEPADS) return NULL;
    notepad_t* np = &notepads[notepad_count++];
    np->window_id = window_id;
    strncpy(np->filename, filename, 31);
    np->filename[31] = 0;
    np->buffer[0] = 0;
    np->len = 0;
    return np;
}

// Desktop state
uint32_t *desktop_framebuffer;
int desktop_width = VGA_WIDTH;
int desktop_height = VGA_HEIGHT;
int taskbar_height = 16; // Smaller taskbar for VGA 200px height
int desktop_theme = 0; // 0 = Light, 1 = Dark

// VGA color indices for desktop
#define COLOR_BLACK     0x00
#define COLOR_BLUE      0x01
#define COLOR_GREEN     0x02
#define COLOR_CYAN      0x03
#define COLOR_RED       0x04
#define COLOR_MAGENTA   0x05
#define COLOR_BROWN     0x06
#define COLOR_LGRAY     0x07
#define COLOR_DGRAY     0x08
#define COLOR_LBLUE     0x09
#define COLOR_LGREEN    0x0A
#define COLOR_LCYAN     0x0B
#define COLOR_LRED      0x0C
#define COLOR_LMAGENTA  0x0D
#define COLOR_YELLOW    0x0E
#define COLOR_WHITE     0x0F

desktop_icon_t desktop_icons[NUM_ICONS] = {
    {40, 60, 32, 32, "Files", 0},
    {120, 60, 32, 32, "About", 1},
    {200, 60, 32, 32, "Calc", 2},
    {280, 60, 32, 32, "Settings", 3}
};

void desktop_init(uint32_t *fb, int width, int height) {
    (void)width;  // Unused parameter
    (void)height; // Unused parameter
    
    desktop_framebuffer = fb;
    desktop_width = VGA_WIDTH;  // Force VGA dimensions
    desktop_height = VGA_HEIGHT;
    
    // Initialize subsystems
    window_init();
    mouse_init(fb, VGA_WIDTH, VGA_HEIGHT);
    keyboard_init();
    
    // Create some demo windows (smaller to fit VGA resolution)
    window_create(20, 20, 180, 100, "Welcome", COLOR_LGRAY);
    window_create(60, 40, 200, 120, "MyOS", COLOR_WHITE);
}

void desktop_draw_background(void) {
    // Use VGA color indices for better performance
    uint8_t bg_color = (desktop_theme == 0) ? COLOR_BLUE : COLOR_BLACK;
    
    // Fill background with a simple pattern in VGA mode
    for (int y = 0; y < desktop_height - taskbar_height; y++) {
        uint8_t color = bg_color;
        
        // Add slight gradient in light mode
        if (desktop_theme == 0) {
            if (y < 100) color = COLOR_BLUE;      // Dark blue at top
            else color = COLOR_LBLUE;             // Lighter blue at bottom
        }
        
        // Fill the line with the selected color
        uint8_t *vga_buffer = (uint8_t*)desktop_framebuffer;
        for (int x = 0; x < desktop_width; x++) {
            vga_buffer[y * desktop_width + x] = color;
        }
    }
}

void desktop_draw_icons(void) {
    for (int i = 0; i < NUM_ICONS; i++) {
        desktop_icon_t *icon = &desktop_icons[i];
        uint8_t icon_color = (i % 2 == 0) ? COLOR_YELLOW : COLOR_LGREEN; // Yellow or Light Green
        
        // Draw icon square (simplified for VGA)
        for (int y = 0; y < icon->height; y++) {
            for (int x = 0; x < icon->width; x++) {
                int px = icon->x + x;
                int py = icon->y + y;
                if (px >= 0 && px < desktop_width && py >= 0 && py < desktop_height - taskbar_height) {
                    // Simple border and fill
                    if (x == 0 || x == icon->width-1 || y == 0 || y == icon->height-1) {
                        ((uint8_t*)desktop_framebuffer)[py * desktop_width + px] = 0x0F; // White border
                    } else {
                        ((uint8_t*)desktop_framebuffer)[py * desktop_width + px] = icon_color;
                    }
                }
            }
        }
        // Draw icon label (white text)
        draw_string(icon->label, icon->x, icon->y + icon->height + 4, COLOR_WHITE);
    }
}

void desktop_draw_taskbar(void) {
    int taskbar_y = desktop_height - taskbar_height;
    
    // Draw taskbar background (dark gray)
    uint8_t taskbar_color = (desktop_theme == 0) ? COLOR_LGRAY : COLOR_DGRAY;
    for (int y = taskbar_y; y < desktop_height; y++) {
        for (int x = 0; x < desktop_width; x++) {
            ((uint8_t*)desktop_framebuffer)[y * desktop_width + x] = taskbar_color;
        }
    }
    
    // Draw taskbar border (light gray)
    for (int x = 0; x < desktop_width; x++) {
        ((uint8_t*)desktop_framebuffer)[taskbar_y * desktop_width + x] = COLOR_LGRAY; // Light gray
    }
    
    // Draw start button (simplified for VGA)
    for (int y = taskbar_y + 1; y < desktop_height - 1; y++) {
        for (int x = 2; x < 50; x++) {
            ((uint8_t*)desktop_framebuffer)[y * desktop_width + x] = COLOR_LGRAY;
        }
    }
    draw_string("Start", 10, taskbar_y + 4, COLOR_BLACK); // Black text on light gray
    
    // Draw AI button (right side, simplified)
    int ai_btn_width = 40;
    int ai_btn_x = desktop_width - ai_btn_width - 5;
    for (int y = taskbar_y + 2; y < desktop_height - 2; y++) {
        for (int x = ai_btn_x; x < ai_btn_x + ai_btn_width; x++) {
            ((uint8_t*)desktop_framebuffer)[y * desktop_width + x] = COLOR_LBLUE; // Light blue
        }
    }
    draw_string("AI", ai_btn_x + 15, taskbar_y + 4, COLOR_BLACK); // Black text on blue
    
    // Draw window buttons in taskbar (simplified for VGA)
    int button_x = 55;
    for (int i = 0; i < window_count; i++) {
        if (windows[i].active) {
            uint8_t btn_color = (i == active_window) ? COLOR_LGRAY : COLOR_DGRAY; // Light gray or dark gray
            
            // Draw button background
            for (int y = taskbar_y + 1; y < desktop_height - 1; y++) {
                for (int x = button_x; x < button_x + 60; x++) { // Smaller buttons
                    ((uint8_t*)desktop_framebuffer)[y * desktop_width + x] = btn_color;
                }
            }
            
            // Draw window title (truncate if needed)
            char short_title[8];
            int j = 0;
            while (windows[i].title[j] && j < 7) {
                short_title[j] = windows[i].title[j];
                j++;
            }
            short_title[j] = '\0';
            draw_string(short_title, button_x + 2, taskbar_y + 4, 
                       (i == active_window) ? COLOR_BLACK : COLOR_WHITE); // Black on active, white on inactive
                        
            button_x += 65; // Smaller spacing between buttons
        }
    }
}

// Calculator state
static int calc_value = 0;
static int calc_operand = 0;
static char calc_op = 0;
static int calc_new_input = 1;

void draw_calculator(int wx, int wy) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", calc_value);
    draw_string(buf, wx + 20, wy + 30, COLOR_BLACK);
    // Draw buttons (4x5 grid)
    const char *labels[5][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"0", "+", "=", "C"},
        {"",  "",  "",  ""}
    };
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int bx = wx + 20 + col * 40;
            int by = wy + 60 + row * 28;
            uint8_t* vga_buffer = (uint8_t*)desktop_framebuffer;
            for (int y = 0; y < 24; y++) {
                for (int x = 0; x < 36; x++) {
                    vga_buffer[(by + y) * desktop_width + (bx + x)] = COLOR_LGRAY; // Light gray buttons
                }
            }
            draw_string(labels[row][col], bx + 12, by + 6, COLOR_BLACK);
        }
    }
}

void desktop_draw(void) {
    desktop_draw_background();
    desktop_draw_icons();
    window_draw_all(desktop_framebuffer, desktop_width, desktop_height);
    desktop_draw_taskbar();
    mouse_draw(desktop_framebuffer, desktop_width);
    for (int i = 0; i < window_count; i++) {
        if (windows[i].active) {
            if (strcmp(windows[i].title, "AI Assistant") == 0) {
                draw_string("How can I help you?", windows[i].x + 20, windows[i].y + 40, COLOR_BLACK);
            } else if (strcmp(windows[i].title, "About") == 0) {
                draw_string("MyOS v0.1", windows[i].x + 20, windows[i].y + 40, COLOR_BLACK);
                draw_string("Created by Vinay", windows[i].x + 20, windows[i].y + 60, COLOR_BLACK);
            } else if (strcmp(windows[i].title, "File Explorer") == 0) {
                draw_string("Files:", windows[i].x + 20, windows[i].y + 40, COLOR_BLACK);
                draw_string("- readme.txt", windows[i].x + 40, windows[i].y + 60, COLOR_DGRAY);
                draw_string("- notes.txt", windows[i].x + 40, windows[i].y + 80, COLOR_DGRAY);
            } else if (strncmp(windows[i].title, "Notepad:", 8) == 0) {
                notepad_t* np = find_notepad(i);
                if (np) {
                    draw_string(np->filename, windows[i].x + 20, windows[i].y + 30, COLOR_BLACK);
                    draw_string(np->buffer, windows[i].x + 20, windows[i].y + 50, COLOR_BLACK);
                } else {
                    draw_string("(Notepad stub)", windows[i].x + 20, windows[i].y + 40, COLOR_BLACK);
                }
            } else if (strcmp(windows[i].title, "Calculator") == 0) {
                draw_calculator(windows[i].x, windows[i].y);
            } else if (strcmp(windows[i].title, "Settings") == 0) {
                draw_string("Settings", windows[i].x + 20, windows[i].y + 40, COLOR_BLACK);
                draw_string(desktop_theme == 0 ? "Theme: Light" : "Theme: Dark", 
                          windows[i].x + 20, windows[i].y + 60, COLOR_DGRAY);
                draw_string("Version: 0.1", windows[i].x + 20, windows[i].y + 80, COLOR_DGRAY);
                // Draw toggle button
                int btn_x = windows[i].x + 20, btn_y = windows[i].y + 100;
                uint8_t* vga_buffer = (uint8_t*)desktop_framebuffer;
                for (int y = 0; y < 24; y++) {
                    for (int x = 0; x < 80; x++) {
                        vga_buffer[(btn_y + y) * desktop_width + (btn_x + x)] = COLOR_LBLUE; // Light blue button
                    }
                }
                draw_string("Toggle Theme", btn_x + 6, btn_y + 6, COLOR_BLACK);
            }
        }
    }
}

void handle_calculator_click(int wx, int wy, int x, int y) {
    int btn_row = (y - (wy + 60)) / 28;
    int btn_col = (x - (wx + 20)) / 40;
    if (btn_row < 0 || btn_row > 3 || btn_col < 0 || btn_col > 3) return;
    const char *labels[4][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"0", "+", "=", "C"}
    };
    const char *label = labels[btn_row][btn_col];
    if (!label[0]) return;
    if (label[0] >= '0' && label[0] <= '9') {
        int digit = label[0] - '0';
        if (calc_new_input) {
            calc_value = digit;
            calc_new_input = 0;
        } else {
            calc_value = calc_value * 10 + digit;
        }
    } else if (label[0] == '+') {
        calc_operand = calc_value;
        calc_op = '+';
        calc_new_input = 1;
    } else if (label[0] == '-') {
        calc_operand = calc_value;
        calc_op = '-';
        calc_new_input = 1;
    } else if (label[0] == '*') {
        calc_operand = calc_value;
        calc_op = '*';
        calc_new_input = 1;
    } else if (label[0] == '/') {
        calc_operand = calc_value;
        calc_op = '/';
        calc_new_input = 1;
    } else if (label[0] == '=') {
        if (calc_op == '+') calc_value = calc_operand + calc_value;
        else if (calc_op == '-') calc_value = calc_operand - calc_value;
        else if (calc_op == '*') calc_value = calc_operand * calc_value;
        else if (calc_op == '/') calc_value = (calc_value != 0) ? (calc_operand / calc_value) : 0;
        calc_op = 0;
        calc_new_input = 1;
    } else if (label[0] == 'C') {
        calc_value = 0;
        calc_operand = 0;
        calc_op = 0;
        calc_new_input = 1;
    }
}

void handle_calculator_key(char key) {
    if (key >= '0' && key <= '9') {
        int digit = key - '0';
        if (calc_new_input) {
            calc_value = digit;
            calc_new_input = 0;
        } else {
            calc_value = calc_value * 10 + digit;
        }
    } else if (key == '+') {
        calc_operand = calc_value;
        calc_op = '+';
        calc_new_input = 1;
    } else if (key == '-') {
        calc_operand = calc_value;
        calc_op = '-';
        calc_new_input = 1;
    } else if (key == '*') {
        calc_operand = calc_value;
        calc_op = '*';
        calc_new_input = 1;
    } else if (key == '/') {
        calc_operand = calc_value;
        calc_op = '/';
        calc_new_input = 1;
    } else if (key == '=' || key == '\n') {
        if (calc_op == '+') calc_value = calc_operand + calc_value;
        else if (calc_op == '-') calc_value = calc_operand - calc_value;
        else if (calc_op == '*') calc_value = calc_operand * calc_value;
        else if (calc_op == '/') calc_value = (calc_value != 0) ? (calc_operand / calc_value) : 0;
        calc_op = 0;
        calc_new_input = 1;
    } else if (key == 'C' || key == 'c' || key == '\b') {
        calc_value = 0;
        calc_operand = 0;
        calc_op = 0;
        calc_new_input = 1;
    }
}

void desktop_handle_mouse_click(int x, int y, int button) {
    if (button == 1) { // Left click
        // Check if clicking on an icon
        for (int i = 0; i < NUM_ICONS; i++) {
            desktop_icon_t *icon = &desktop_icons[i];
            if (x >= icon->x && x < icon->x + icon->width &&
                y >= icon->y && y < icon->y + icon->height) {
                if (icon->type == 0) {
                    window_create(200, 120, 220, 140, "File Explorer", 0x0F); // White
                    return;
                } else if (icon->type == 1) {
                    window_create(240, 180, 180, 100, "About", 0x0E); // Yellow
                    return;
                } else if (icon->type == 2) {
                    window_create(300, 200, 180, 120, "Calculator", 0x0A); // Light Green
                    // Reset calculator state
                    calc_value = 0; calc_operand = 0; calc_op = 0; calc_new_input = 1;
                    return;
                } else if (icon->type == 3) {
                    window_create(350, 220, 200, 120, "Settings", 0x09); // Light Blue
                    return;
                }
            }
        }
        // Check if clicking in taskbar
        int ai_btn_width = 60;
        int ai_btn_x = desktop_width - ai_btn_width - 10;
        if (y >= desktop_height - taskbar_height) {
            if (x >= ai_btn_x && x < ai_btn_x + ai_btn_width) {
                window_create(desktop_width/2-120, desktop_height/2-60, 240, 120, "AI Assistant", 0x0B); // Light Cyan
                return;
            }
            if (x >= 2 && x < 80 && y >= desktop_height - taskbar_height + 2 && y < desktop_height - 2) {
                window_create(100, 100, 200, 150, "New Window", 0x07); // Light Gray
            } else if (x >= 90) {
                int window_index = (x - 90) / 110;
                if (window_index < window_count) {
                    if (windows[window_index].minimized) {
                        windows[window_index].minimized = 0;
                    }
                    window_bring_to_front(window_index);
                }
            }
            return;
        }
        // Check if clicking on a window control button
        int win_id = window_at_position(x, y);
        if (win_id >= 0) {
            window_t *win = &windows[win_id];
            int wx = win->maximized ? 0 : win->x;
            int wy = win->maximized ? 0 : win->y;
            int wwidth = win->maximized ? desktop_width : win->width;
            // Only check in title bar
            if (y >= wy + 2 && y < wy + 14) {
                int btn_x = wx + wwidth - 50;
                // Close
                if (x >= btn_x && x < btn_x + 12) {
                    window_destroy(win_id);
                    return;
                }
                // Minimize
                if (x >= btn_x + 16 && x < btn_x + 28) {
                    win->minimized = 1;
                    return;
                }
                // Maximize
                if (x >= btn_x + 32 && x < btn_x + 44) {
                    win->maximized = !win->maximized;
                    return;
                }
            }
        }
        // Check if clicking on a window
        int window_id = window_at_position(x, y);
        if (window_id >= 0) {
            window_bring_to_front(window_id);
            window_start_drag(window_id, x, y);
        }
        // Check if clicking on a file in File Explorer
        for (int i = 0; i < window_count; i++) {
            if (windows[i].active && strcmp(windows[i].title, "File Explorer") == 0) {
                int wx = windows[i].maximized ? 0 : windows[i].x;
                int wy = windows[i].maximized ? 0 : windows[i].y;
                if (x >= wx + 40 && x < wx + 200 && y >= wy + 60 && y < wy + 76) {
                    int win_id = window_create(wx + 100, wy + 40, 180, 80, "Notepad: readme.txt", 0x0E); // Yellow
                    create_notepad(win_id, "readme.txt");
                    return;
                }
                if (x >= wx + 40 && x < wx + 200 && y >= wy + 80 && y < wy + 96) {
                    int win_id = window_create(wx + 100, wy + 60, 180, 80, "Notepad: notes.txt", 0x0E); // Yellow
                    create_notepad(win_id, "notes.txt");
                    return;
                }
            }
        }
        // Calculator button clicks
        for (int i = 0; i < window_count; i++) {
            if (windows[i].active && strcmp(windows[i].title, "Calculator") == 0) {
                int wx = windows[i].maximized ? 0 : windows[i].x;
                int wy = windows[i].maximized ? 0 : windows[i].y;
                if (x >= wx + 20 && x < wx + 20 + 4 * 40 && y >= wy + 60 && y < wy + 60 + 4 * 28) {
                    handle_calculator_click(wx, wy, x, y);
                    return;
                }
            }
        }
        // Settings toggle theme button
        for (int i = 0; i < window_count; i++) {
            if (windows[i].active && strcmp(windows[i].title, "Settings") == 0) {
                int btn_x = windows[i].x + 20, btn_y = windows[i].y + 100;
                if (x >= btn_x && x < btn_x + 80 && y >= btn_y && y < btn_y + 24) {
                    desktop_theme = !desktop_theme;
                    return;
                }
            }
        }
    }
}

void desktop_handle_mouse_move(int x, int y) {
    // Update mouse position
    mouse_set_position(x, y);
    
    // Update window dragging
    window_update_drag(x, y);
}

void desktop_handle_keyboard_input(char key) {
    if (key == 0) return;
    if (active_window >= 0 && strncmp(windows[active_window].title, "Notepad:", 8) == 0) {
        notepad_t* np = find_notepad(active_window);
        if (np) {
            if (key == '\b') {
                if (np->len > 0) {
                    np->len--;
                    np->buffer[np->len] = 0;
                }
            } else if (key >= 32 && key <= 126 && np->len < NOTEPAD_BUF_SIZE - 1) {
                np->buffer[np->len++] = key;
                np->buffer[np->len] = 0;
            }
        }
        return;
    }
    if (active_window >= 0 && strcmp(windows[active_window].title, "Calculator") == 0) {
        handle_calculator_key(key);
        return;
    }
    // Handle special keys
    switch (key) {
        case '\n': // Enter key
            // Create a new window
            window_create(150, 150, 250, 200, "Keyboard Input", 0x07); // Light Gray
            break;
        case '\b': // Backspace
            // Close active window
            if (active_window >= 0) {
                window_destroy(active_window);
            }
            break;
        default:
            // Handle regular text input
            if (active_window >= 0 && strncmp(windows[active_window].title, "Notepad:", 8) == 0) {
                notepad_t* np = find_notepad(active_window);
                if (np) {
                    if (key >= 32 && key <= 126 && np->len < NOTEPAD_BUF_SIZE - 1) {
                        np->buffer[np->len++] = key;
                        np->buffer[np->len] = 0;
                    } else if (key == '\b' && np->len > 0) {
                        np->len--;
                        np->buffer[np->len] = 0;
                    }
                }
            }
            break;
    }
}

void desktop_handle_input(void) {
    // Handle keyboard input
    while (keyboard_has_data()) {
        char key = keyboard_get_char();
        desktop_handle_keyboard_input(key);
    }
    
    // Handle mouse input (simplified - in real implementation you'd read from PS/2)
    // For now, we'll just update the mouse position based on some simulation
    
    // Simulate mouse movement (for demo purposes)
    static int sim_x = 100;
    static int sim_y = 100;
    static int sim_dx = 1;
    static int sim_dy = 1;
    
    sim_x += sim_dx;
    sim_y += sim_dy;
    
    if (sim_x <= 0 || sim_x >= desktop_width - 12) sim_dx = -sim_dx;
    if (sim_y <= 0 || sim_y >= desktop_height - taskbar_height - 12) sim_dy = -sim_dy;
    
    mouse_set_position(sim_x, sim_y);
} 