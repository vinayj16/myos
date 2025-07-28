#include <stdint.h>
#include <string.h>
#include "window.h"
#include "text.h"
#include "framebuffer.h"  // For VGA color definitions

// Window state
window_t windows[MAX_WINDOWS];
int window_count = 0;
int active_window = -1;
int dragging_window = -1;

void window_init(void) {
    window_count = 0;
    active_window = -1;
    dragging_window = -1;
    
    for (int i = 0; i < MAX_WINDOWS; i++) {
        windows[i].active = 0;
        windows[i].dragging = 0;
    }
}

int window_create(int16_t x, int16_t y, uint16_t width, uint16_t height, const char *title, uint8_t color) {
    if (window_count >= MAX_WINDOWS) {
        return -1; // No more window slots
    }
    int id = window_count++;
    windows[id].x = x;
    windows[id].y = y;
    windows[id].width = width;
    windows[id].height = height;
    windows[id].color = color;
    windows[id].active = 1;
    windows[id].dragging = 0;
    windows[id].minimized = 0;
    windows[id].maximized = 0;
    int i = 0;
    while (title[i] && i < 63) {
        windows[id].title[i] = title[i];
        i++;
    }
    windows[id].title[i] = '\0';
    window_bring_to_front(id);
    return id;
}

void window_destroy(int id) {
    if (id < 0 || id >= window_count || !windows[id].active) {
        return;
    }
    
    // Remove window by shifting others
    for (int i = id; i < window_count - 1; i++) {
        windows[i] = windows[i + 1];
    }
    window_count--;
    
    // Update active window
    if (active_window == id) {
        active_window = (window_count > 0) ? 0 : -1;
    } else if (active_window > id) {
        active_window--;
    }
}

void window_draw(uint32_t *fb, int width, int height, int id) {
    if (id < 0 || id >= window_count || !windows[id].active || windows[id].minimized) {
        return;
    }
    
    window_t *win = &windows[id];
    int wx = win->x, wy = win->y, wwidth = win->width, wheight = win->height;
    
    // For maximized windows, adjust dimensions but leave space for taskbar
    if (win->maximized) {
        wx = 0; 
        wy = 0; 
        wwidth = VGA_WIDTH; 
        wheight = VGA_HEIGHT - 16; // Leave space for taskbar
    }
    
    // Clip to screen bounds
    if (wx < 0) { wwidth += wx; wx = 0; }
    if (wy < 0) { wheight += wy; wy = 0; }
    if (wx + wwidth > VGA_WIDTH) wwidth = VGA_WIDTH - wx;
    if (wy + wheight > VGA_HEIGHT) wheight = VGA_HEIGHT - wy;
    
    if (wwidth <= 0 || wheight <= 0) return;
    
    // Convert framebuffer to byte pointer for 8-bit access
    uint8_t *vga_buffer = (uint8_t *)fb;
    
    // Draw window background (using 8-bit color index)
    uint8_t bg_color = win->color & 0xFF; // Use lower 8 bits as VGA color index
    for (int y = 0; y < wheight; y++) {
        for (int x = 0; x < wwidth; x++) {
            vga_buffer[(wy + y) * width + (wx + x)] = bg_color;
        }
    }
    
    // Draw window border (1px)
    uint8_t border_color = (id == active_window) ? 0x0F : 0x08; // White or dark gray
    
    // Top border
    for (int x = 0; x < wwidth; x++) {
        vga_buffer[wy * width + (wx + x)] = border_color;
    }
    
    // Bottom border
    for (int x = 0; x < wwidth; x++) {
        if (wy + wheight - 1 < VGA_HEIGHT) {
            vga_buffer[(wy + wheight - 1) * width + (wx + x)] = border_color;
        }
    }
    
    // Left border
    for (int y = 0; y < wheight; y++) {
        vga_buffer[(wy + y) * width + wx] = border_color;
    }
    
    // Right border
    for (int y = 0; y < wheight; y++) {
        if (wx + wwidth - 1 < VGA_WIDTH) {
            vga_buffer[(wy + y) * width + (wx + wwidth - 1)] = border_color;
        }
    }
    
    // Draw title bar (simplified for VGA)
    int title_height = 12; // Smaller title bar for VGA
    uint8_t title_color = (id == active_window) ? 0x01 : 0x08; // Blue or dark gray
    
    // Title bar background
    for (int y = 1; y < title_height - 1; y++) {
        for (int x = 1; x < wwidth - 1; x++) {
            if (wx + x < VGA_WIDTH && wy + y < VGA_HEIGHT) {
                vga_buffer[(wy + y) * width + (wx + x)] = title_color;
            }
        }
    }
    
    // Draw title text (centered, with clipping)
    int title_x = wx + 4;
    int title_y = wy + 2;
    
    // Only draw if the title position is on screen
    if (title_x < VGA_WIDTH && title_y < VGA_HEIGHT) {
        // Truncate title if too long for window
        char title[32];
        int max_len = (wwidth - 8) / 8; // Approximate based on font width
        if (max_len > 31) max_len = 31;
        
        strncpy(title, win->title, max_len);
        title[max_len] = '\0';
        
        // Draw with white (0x0F) or black (0x00) text based on title color
        uint8_t text_color = (title_color < 0x08) ? 0x0F : 0x00;
        draw_string(title, title_x, title_y, text_color);
    }
    
    // Draw window control buttons (right side, simplified for VGA)
    int btn_y = wy + 2;
    int btn_x = wx + wwidth - 40; // Move left to fit in VGA
    
    // Only draw buttons if there's enough space
    if (btn_x >= 0 && btn_x + 36 < VGA_WIDTH && btn_y + 10 < VGA_HEIGHT) {
        uint8_t *vga_buffer = (uint8_t *)fb;
        
        // Close button (red)
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                if (btn_x + x < VGA_WIDTH && btn_y + y < VGA_HEIGHT) {
                    vga_buffer[(btn_y + y) * width + (btn_x + x)] = 0x04; // Red
                }
            }
        }
        
        // Minimize button (yellow) - only show if not maximized
        if (!win->maximized) {
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    if (btn_x + 12 + x < VGA_WIDTH && btn_y + y < VGA_HEIGHT) {
                        vga_buffer[(btn_y + y) * width + (btn_x + 12 + x)] = 0x0E; // Yellow
                    }
                }
            }
        }
        
        // Maximize/restore button (green/blue)
        uint8_t max_color = win->maximized ? 0x01 : 0x02; // Blue for restore, green for maximize
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                if (btn_x + 24 + x < VGA_WIDTH && btn_y + y < VGA_HEIGHT) {
                    vga_buffer[(btn_y + y) * width + (btn_x + 24 + x)] = max_color;
                }
            }
        }
    }
}

void window_draw_all(uint32_t *fb, int width, int height) {
    // Draw windows in reverse order (back to front)
    for (int i = window_count - 1; i >= 0; i--) {
        window_draw(fb, width, height, i);
    }
}

int window_at_position(int16_t x, int16_t y) {
    // Check windows in reverse order (front to back)
    for (int i = window_count - 1; i >= 0; i--) {
        if (windows[i].active) {
            if (x >= windows[i].x && x < windows[i].x + windows[i].width &&
                y >= windows[i].y && y < windows[i].y + windows[i].height) {
                return i;
            }
        }
    }
    return -1;
}

void window_bring_to_front(int id) {
    if (id < 0 || id >= window_count || !windows[id].active) {
        return;
    }
    
    // Move window to end of list (front)
    window_t temp = windows[id];
    for (int i = id; i < window_count - 1; i++) {
        windows[i] = windows[i + 1];
    }
    windows[window_count - 1] = temp;
    
    active_window = window_count - 1;
}

void window_start_drag(int id, int16_t x, int16_t y) {
    if (id < 0 || id >= window_count || !windows[id].active) {
        return;
    }
    
    // Only allow dragging from title bar
    if (y < windows[id].y + WINDOW_TITLE_HEIGHT) {
        windows[id].dragging = 1;
        windows[id].drag_start_x = x - windows[id].x;
        windows[id].drag_start_y = y - windows[id].y;
        windows[id].original_x = windows[id].x;
        windows[id].original_y = windows[id].y;
        dragging_window = id;
    }
}

void window_stop_drag(void) {
    if (dragging_window >= 0 && dragging_window < window_count) {
        windows[dragging_window].dragging = 0;
        dragging_window = -1;
    }
}

void window_update_drag(int16_t x, int16_t y) {
    if (dragging_window >= 0 && dragging_window < window_count && windows[dragging_window].dragging) {
        windows[dragging_window].x = x - windows[dragging_window].drag_start_x;
        windows[dragging_window].y = y - windows[dragging_window].drag_start_y;
        
        // Clamp to screen boundaries
        if (windows[dragging_window].x < 0) windows[dragging_window].x = 0;
        if (windows[dragging_window].y < 0) windows[dragging_window].y = 0;
    }
}
