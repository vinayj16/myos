#ifndef _SHORTCUTS_H
#define _SHORTCUTS_H

#include <stdint.h>

// Modifier keys
#define MOD_CTRL  0x01
#define MOD_ALT   0x02
#define MOD_SHIFT 0x04
#define MOD_WIN   0x08

// Special keys
#define KEY_F1    0x80
#define KEY_F2    0x81
#define KEY_F3    0x82
#define KEY_F4    0x83
#define KEY_F5    0x84
#define KEY_F6    0x85
#define KEY_F7    0x86
#define KEY_F8    0x87
#define KEY_F9    0x88
#define KEY_F10   0x89
#define KEY_F11   0x8A
#define KEY_F12   0x8B

typedef struct {
    uint8_t modifiers;
    uint8_t key;
    void (*handler)(void);
    char description[64];
} shortcut_t;

// Shortcut system functions
void shortcuts_init(void);
void shortcuts_register(uint8_t modifiers, uint8_t key, void (*handler)(void), const char* description);
int shortcuts_handle_key(uint8_t modifiers, uint8_t key);
void shortcuts_show_help(void);

// Built-in shortcut handlers
void shortcut_show_desktop(void);
void shortcut_open_terminal(void);
void shortcut_open_file_manager(void);
void shortcut_open_system_monitor(void);
void shortcut_toggle_theme(void);
void shortcut_screenshot(void);
void shortcut_lock_screen(void);
void shortcut_show_run_dialog(void);
void shortcut_close_window(void);
void shortcut_minimize_window(void);
void shortcut_maximize_window(void);

#endif // _SHORTCUTS_H