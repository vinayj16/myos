#ifndef _DESKTOP_H
#define _DESKTOP_H

#include <stdint.h>

#define NUM_ICONS 4

// Desktop icon structure
typedef struct {
    int x, y;
    int width, height;
    char label[32];
    int type; // 0 = folder, 1 = application, 2 = file
} desktop_icon_t;

// Function declarations
void desktop_init(uint32_t *fb, int width, int height);
void desktop_draw(void);
void desktop_draw_background(void);
void desktop_draw_icons(void);
void desktop_draw_taskbar(void);
void desktop_handle_mouse_click(int x, int y, int button);
void desktop_handle_mouse_move(int x, int y);
void desktop_handle_keyboard_input(char key);
void desktop_handle_input(void);

// Global desktop icons
extern desktop_icon_t desktop_icons[NUM_ICONS];

#endif // _DESKTOP_H