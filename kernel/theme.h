#ifndef _THEME_H
#define _THEME_H

#include <stdint.h>

// Theme system for MyOS
typedef struct {
    uint32_t background_primary;
    uint32_t background_secondary;
    uint32_t window_background;
    uint32_t window_border;
    uint32_t window_title_active;
    uint32_t window_title_inactive;
    uint32_t text_primary;
    uint32_t text_secondary;
    uint32_t accent_color;
    uint32_t taskbar_background;
    uint32_t taskbar_text;
    uint32_t button_background;
    uint32_t button_hover;
    uint32_t button_pressed;
} theme_t;

// Predefined themes
extern theme_t light_theme;
extern theme_t dark_theme;
extern theme_t current_theme;

// Theme functions
void theme_init(void);
void theme_set_light(void);
void theme_set_dark(void);
void theme_toggle(void);
uint32_t theme_get_color(const char* color_name);

#endif // _THEME_H