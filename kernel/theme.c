#include "theme.h"
#include "libc/string.h"

// Light theme colors
theme_t light_theme = {
    .background_primary = 0x4A90E2,     // Light blue
    .background_secondary = 0x5BA0F2,   // Lighter blue
    .window_background = 0xF5F5F5,      // Light gray
    .window_border = 0xD0D0D0,          // Gray border
    .window_title_active = 0x3366CC,    // Blue title bar
    .window_title_inactive = 0x888888,  // Gray title bar
    .text_primary = 0x000000,           // Black text
    .text_secondary = 0x666666,         // Gray text
    .accent_color = 0x007ACC,           // Blue accent
    .taskbar_background = 0x2D2D30,     // Dark taskbar
    .taskbar_text = 0xFFFFFF,           // White text
    .button_background = 0xE1E1E1,      // Light button
    .button_hover = 0xD4D4D4,           // Hover state
    .button_pressed = 0xC7C7C7          // Pressed state
};

// Dark theme colors
theme_t dark_theme = {
    .background_primary = 0x1E1E1E,     // Dark gray
    .background_secondary = 0x2D2D30,   // Slightly lighter
    .window_background = 0x252526,      // Dark window
    .window_border = 0x3E3E42,          // Dark border
    .window_title_active = 0x007ACC,    // Blue title bar
    .window_title_inactive = 0x3E3E42,  // Dark gray title bar
    .text_primary = 0xFFFFFF,           // White text
    .text_secondary = 0xCCCCCC,         // Light gray text
    .accent_color = 0x0E639C,           // Dark blue accent
    .taskbar_background = 0x1E1E1E,     // Very dark taskbar
    .taskbar_text = 0xFFFFFF,           // White text
    .button_background = 0x3C3C3C,      // Dark button
    .button_hover = 0x464647,           // Hover state
    .button_pressed = 0x525252          // Pressed state
};

// Current active theme
theme_t current_theme;
static int is_dark_theme = 0;

void theme_init(void) {
    // Start with light theme
    current_theme = light_theme;
    is_dark_theme = 0;
}

void theme_set_light(void) {
    current_theme = light_theme;
    is_dark_theme = 0;
}

void theme_set_dark(void) {
    current_theme = dark_theme;
    is_dark_theme = 1;
}

void theme_toggle(void) {
    if (is_dark_theme) {
        theme_set_light();
    } else {
        theme_set_dark();
    }
}

uint32_t theme_get_color(const char* color_name) {
    if (strcmp(color_name, "background_primary") == 0) return current_theme.background_primary;
    if (strcmp(color_name, "background_secondary") == 0) return current_theme.background_secondary;
    if (strcmp(color_name, "window_background") == 0) return current_theme.window_background;
    if (strcmp(color_name, "window_border") == 0) return current_theme.window_border;
    if (strcmp(color_name, "window_title_active") == 0) return current_theme.window_title_active;
    if (strcmp(color_name, "window_title_inactive") == 0) return current_theme.window_title_inactive;
    if (strcmp(color_name, "text_primary") == 0) return current_theme.text_primary;
    if (strcmp(color_name, "text_secondary") == 0) return current_theme.text_secondary;
    if (strcmp(color_name, "accent_color") == 0) return current_theme.accent_color;
    if (strcmp(color_name, "taskbar_background") == 0) return current_theme.taskbar_background;
    if (strcmp(color_name, "taskbar_text") == 0) return current_theme.taskbar_text;
    if (strcmp(color_name, "button_background") == 0) return current_theme.button_background;
    if (strcmp(color_name, "button_hover") == 0) return current_theme.button_hover;
    if (strcmp(color_name, "button_pressed") == 0) return current_theme.button_pressed;
    
    return 0xFFFFFF; // Default to white
}
