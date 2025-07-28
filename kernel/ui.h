#ifndef _UI_H
#define _UI_H

#include <stdint.h>
#include <stddef.h>

// UI Constants
#define MAX_WINDOWS 32
#define TASKBAR_HEIGHT 30
#define TASKBAR_ITEM_WIDTH 120

// UI Colors
#define UI_COLOR_PRIMARY   0x2196F3
#define UI_COLOR_SECONDARY 0x4CAF50
#define UI_COLOR_ACCENT    0xFF9800
#define UI_COLOR_BACKGROUND 0xF5F5F5
#define UI_COLOR_SURFACE   0xFFFFFF
#define UI_COLOR_ERROR     0xF44336

#define TASKBAR_COLOR      0x263238
#define TASKBAR_TEXT_COLOR 0xFFFFFF

// Window structure
typedef struct {
    int x, y;
    uint32_t width, height;
    char title[64];
    uint32_t color;
    uint32_t bg_color;
    uint32_t border_color;
    uint32_t title_bar_color;
    uint32_t title_text_color;
    int visible;
    int active;
    int dragging;
    int minimized;
    int maximized;
    int drag_start_x, drag_start_y;
} ui_window_t;

// UI State
typedef struct {
    uint32_t *framebuffer;
    uint32_t width, height;
    int taskbar_visible;
    uint32_t taskbar_height;
} ui_state_t;

// Function declarations
void ui_init(uint32_t *fb, uint32_t width, uint32_t height);
void ui_update(void);
void ui_handle_click(int x, int y, int buttons);
void ui_handle_mouse_move(int x, int y, int buttons);
void ui_draw_string(const char *str, int x, int y, uint32_t color);
int ui_create_window(int x, int y, int width, int height, const char *title, uint32_t color);

// Taskbar functions
void taskbar_init(void);
void taskbar_draw(void);
void taskbar_add_icon(const char *name, void (*on_click)(void));
void taskbar_handle_click(int x, int y);

// Window management
void wm_add_window(ui_window_t *window);
void wm_draw_all(void);

#endif // _UI_H
