#include <stdint.h>
#include "keyboard.h"

// PS/2 I/O ports
#define PS2_DATA_PORT    0x60
#define PS2_COMMAND_PORT 0x64

// Keyboard state
uint8_t keyboard_buffer[256];
int keyboard_buffer_head = 0;
int keyboard_buffer_tail = 0;
uint8_t keyboard_shift_pressed = 0;
uint8_t keyboard_ctrl_pressed = 0;
uint8_t keyboard_alt_pressed = 0;

// Scancode to ASCII conversion tables
static const char scancode_table[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static const char scancode_table_shift[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
    0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

// Inline assembly functions for I/O
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void keyboard_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) {
            if ((inb(PS2_COMMAND_PORT) & 1) == 1) {
                return;
            }
        }
    } else {
        while (timeout--) {
            if ((inb(PS2_COMMAND_PORT) & 2) == 0) {
                return;
            }
        }
    }
}

void keyboard_write(uint8_t data) {
    keyboard_wait(1);
    outb(PS2_COMMAND_PORT, 0xD4);
    keyboard_wait(1);
    outb(PS2_DATA_PORT, data);
}

uint8_t keyboard_read(void) {
    keyboard_wait(0);
    return inb(PS2_DATA_PORT);
}

void keyboard_init(void) {
    // Clear keyboard buffer
    keyboard_buffer_head = 0;
    keyboard_buffer_tail = 0;
    keyboard_shift_pressed = 0;
    keyboard_ctrl_pressed = 0;
    keyboard_alt_pressed = 0;
    
    // Reset keyboard
    keyboard_write(0xFF);
    keyboard_read(); // ACK
    
    // Set defaults
    keyboard_write(0xF6);
    keyboard_read(); // ACK
    
    // Enable keyboard
    keyboard_write(0xF4);
    keyboard_read(); // ACK
}

void keyboard_enable(void) {
    keyboard_write(0xF4);
    keyboard_read(); // ACK
}

void keyboard_disable(void) {
    keyboard_write(0xF5);
    keyboard_read(); // ACK
}

void keyboard_process_scancode(uint8_t scancode) {
    // Check if it's a key release (bit 7 set)
    if (scancode & 0x80) {
        uint8_t key = scancode & 0x7F;
        
        // Handle modifier keys
        switch (key) {
            case KEY_SHIFT_LEFT:
            case KEY_SHIFT_RIGHT:
                keyboard_shift_pressed = 0;
                break;
            case KEY_CTRL_LEFT:
                keyboard_ctrl_pressed = 0;
                break;
            case KEY_ALT_LEFT:
                keyboard_alt_pressed = 0;
                break;
        }
        return;
    }
    
    // Handle modifier keys
    switch (scancode) {
        case KEY_SHIFT_LEFT:
        case KEY_SHIFT_RIGHT:
            keyboard_shift_pressed = 1;
            return;
        case KEY_CTRL_LEFT:
            keyboard_ctrl_pressed = 1;
            return;
        case KEY_ALT_LEFT:
            keyboard_alt_pressed = 1;
            return;
    }
    
    // Convert scancode to ASCII
    char ascii = scancode_to_ascii(scancode);
    if (ascii != 0) {
        // Add to buffer
        int next_head = (keyboard_buffer_head + 1) % 256;
        if (next_head != keyboard_buffer_tail) {
            keyboard_buffer[keyboard_buffer_head] = ascii;
            keyboard_buffer_head = next_head;
        }
    }
}

char scancode_to_ascii(uint8_t scancode) {
    if (scancode >= sizeof(scancode_table)) {
        return 0;
    }
    
    if (keyboard_shift_pressed) {
        return scancode_table_shift[scancode];
    } else {
        return scancode_table[scancode];
    }
}

// Get next character from keyboard buffer
char keyboard_get_char(void) {
    if (keyboard_buffer_head == keyboard_buffer_tail) {
        return 0; // Buffer empty
    }
    
    char ch = keyboard_buffer[keyboard_buffer_tail];
    keyboard_buffer_tail = (keyboard_buffer_tail + 1) % 256;
    return ch;
}

// Check if keyboard buffer has data
int keyboard_has_data(void) {
    return keyboard_buffer_head != keyboard_buffer_tail;
} 