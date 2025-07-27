#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <stdint.h>

// Keyboard scan codes
#define KEY_SHIFT_LEFT  0x2A
#define KEY_SHIFT_RIGHT 0x36
#define KEY_CTRL_LEFT   0x1D
#define KEY_ALT_LEFT    0x38

// Function declarations
void keyboard_init(void);
void keyboard_enable(void);
void keyboard_disable(void);
void keyboard_process_scancode(uint8_t scancode);
char scancode_to_ascii(uint8_t scancode);
char keyboard_get_char(void);
int keyboard_has_data(void);

#endif // _KEYBOARD_H