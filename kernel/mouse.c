#include <stdint.h>
#include "graphics.h"
#include "mouse.h"

// Inline assembly functions for I/O
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Mouse state variables
int mouse_x = 100;
int mouse_y = 100;
uint8_t mouse_buttons = 0;

// Mouse cursor bitmap (12x12 arrow cursor)
static uint8_t mouse_cursor[12][12] = {
    {1,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0},
    {1,0,1,0,0,0,0,0,0,0,0,0},
    {1,0,0,1,0,0,0,0,0,0,0,0},
    {1,0,0,0,1,0,0,0,0,0,0,0},
    {1,0,0,0,0,1,0,0,0,0,0,0},
    {1,0,0,0,0,0,1,0,0,0,0,0},
    {1,0,0,0,0,0,0,1,0,0,0,0},
    {1,0,0,0,0,0,0,0,1,0,0,0},
    {1,0,0,0,0,0,0,0,0,1,0,0},
    {1,0,0,0,0,0,0,0,0,0,1,0},
    {1,0,0,0,0,0,0,0,0,0,0,1}
};

static uint32_t *framebuffer;
static int fb_width;
static int fb_height;

// PS/2 I/O ports
#define PS2_DATA_PORT    0x60
#define PS2_COMMAND_PORT 0x64

// Mouse commands
#define MOUSE_CMD_RESET          0xFF
#define MOUSE_CMD_RESEND         0xFE
#define MOUSE_CMD_SET_DEFAULTS   0xF6
#define MOUSE_CMD_DISABLE        0xF5
#define MOUSE_CMD_ENABLE         0xF4
#define MOUSE_CMD_SET_SAMPLE_RATE 0xF3
#define MOUSE_CMD_GET_DEVICE_ID  0xF2
#define MOUSE_CMD_SET_RESOLUTION 0xE8

// Mouse responses
#define MOUSE_ACK        0xFA
#define MOUSE_NACK       0xFE
#define MOUSE_ERROR      0xFC

void mouse_wait(uint8_t type) {
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

void mouse_write(uint8_t data) {
    mouse_wait(1);
    outb(PS2_COMMAND_PORT, 0xD4);
    mouse_wait(1);
    outb(PS2_DATA_PORT, data);
}

uint8_t mouse_read(void) {
    mouse_wait(0);
    return inb(PS2_DATA_PORT);
}

void mouse_init(uint32_t *fb, int width, int height) {
    framebuffer = fb;
    fb_width = width;
    fb_height = height;
    
    // Reset mouse
    mouse_write(MOUSE_CMD_RESET);
    mouse_read(); // ACK
    
    // Set defaults
    mouse_write(MOUSE_CMD_SET_DEFAULTS);
    mouse_read(); // ACK
    
    // Enable mouse
    mouse_write(MOUSE_CMD_ENABLE);
    mouse_read(); // ACK
    
    // Set sample rate to 100
    mouse_write(MOUSE_CMD_SET_SAMPLE_RATE);
    mouse_read(); // ACK
    mouse_write(100);
    mouse_read(); // ACK
}

void mouse_enable(void) {
    mouse_write(MOUSE_CMD_ENABLE);
    mouse_read(); // ACK
}

void mouse_disable(void) {
    mouse_write(MOUSE_CMD_DISABLE);
    mouse_read(); // ACK
}

void mouse_set_position(int x, int y) {
    mouse_x = x;
    mouse_y = y;
    
    // Clamp to screen boundaries
    if (mouse_x < 0) mouse_x = 0;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_x >= fb_width - 12) mouse_x = fb_width - 12;
    if (mouse_y >= fb_height - 12) mouse_y = fb_height - 12;
}

void mouse_get_position(int *x, int *y) {
    *x = mouse_x;
    *y = mouse_y;
}

void mouse_process_packet(int8_t dx, int8_t dy) {
    // Update mouse position
    mouse_x += dx;
    mouse_y -= dy; // Invert Y axis
    
    // Clamp to screen boundaries
    if (mouse_x < 0) mouse_x = 0;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_x >= fb_width - 12) mouse_x = fb_width - 12;
    if (mouse_y >= fb_height - 12) mouse_y = fb_height - 12;
}

void mouse_draw_at(uint32_t *fb, int width, int x, int y) {
    for (int j = 0; j < 12; j++) {
        for (int i = 0; i < 12; i++) {
            if (mouse_cursor[j][i]) {
                int px = x + i;
                int py = y + j;
                if (px >= 0 && px < width && py >= 0 && py < fb_height) {
                    fb[py * width + px] = 0xFFFFFF; // White cursor
                }
            }
        }
    }
}

void mouse_draw(uint32_t *fb, int width) {
    mouse_draw_at(fb, width, mouse_x, mouse_y);
}
