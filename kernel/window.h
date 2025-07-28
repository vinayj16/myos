#ifndef _WINDOW_H
#define _WINDOW_H

#include <stdint.h>
#include "framebuffer.h"  // For VGA_WIDTH, VGA_HEIGHT

#define MAX_WINDOWS 8           // Reduced for VGA memory constraints
#define WINDOW_TITLE_HEIGHT 12  // Smaller title bar for VGA

// Window structure
typedef struct {
    int16_t x, y;           // 16-bit for VGA coordinates (0-319, 0-199)
    uint16_t width, height; // 16-bit for VGA dimensions
    char title[32];         // Shorter title for VGA
    uint8_t color;          // 8-bit color index for VGA
    uint8_t active : 1;     // Bitfield to save space
    uint8_t dragging : 1;
    uint8_t minimized : 1;
    uint8_t maximized : 1;
    int16_t drag_start_x, drag_start_y;
    int16_t original_x, original_y;
} window_t;

// Global window management
extern window_t windows[MAX_WINDOWS];
extern int window_count;
extern int active_window;

// Function declarations
void window_init(void);
int window_create(int16_t x, int16_t y, uint16_t width, uint16_t height, const char *title, uint8_t color);
void window_destroy(int id);
void window_draw(uint32_t *fb, int width, int height, int id);
void window_draw_all(uint32_t *fb, int width, int height);
int window_at_position(int16_t x, int16_t y);
void window_bring_to_front(int id);
void window_start_drag(int id, int16_t x, int16_t y);
void window_stop_drag(void);
void window_update_drag(int16_t x, int16_t y);

#endif // _WINDOW_H
