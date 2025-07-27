#include <stdint.h>
#include "graphics.h"
#include "font.h"  // For draw_string
#include "mouse.h"

// Mouse cursor bitmap (16x16 pixels)
static const uint8_t mouse_cursor[16][16] = {
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0},
    {1,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0},
    {1,2,2,2,2,2,1,1,1,1,1,0,0,0,0,0},
    {1,2,2,1,2,2,1,0,0,0,0,0,0,0,0,0},
    {1,2,1,0,1,2,2,1,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,1,2,2,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,2,2,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,2,2,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0}
};

// Global mouse state variables
int mouse_x = 100;
int mouse_y = 100;
uint8_t mouse_buttons = 0;

// Framebuffer info
static uint32_t *framebuffer;
static int fb_width, fb_height;

#include "io.h"

// Use the I/O functions from io.h
#define outb(port, val) outb(port, val)
#define inb(port) inb(port)

void ps2_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) {
            if ((inb(0x64) & 1) == 1)
                return;
        }
    } else {
        while (timeout--) {
            if ((inb(0x64) & 2) == 0)
                return;
        }
    }
}

void mouse_init(uint32_t *fb, int width, int height) {
    framebuffer = fb;
    fb_width = width;
    fb_height = height;

    ps2_wait(1);
    outb(0x64, 0xA8); // Enable auxiliary device
    ps2_wait(1);
    outb(0x64, 0x20);
    ps2_wait(0);
    uint8_t status = (inb(0x60) | 2);
    ps2_wait(1);
    outb(0x64, 0x60);
    ps2_wait(1);
    outb(0x60, status);

    ps2_wait(1);
    outb(0x64, 0xD4);
    ps2_wait(1);
    outb(0x60, 0xF4);
    inb(0x60); // ACK
}

void mouse_process_packet(int8_t dx, int8_t dy) {
    // Update mouse position
    mouse_x += dx;
    mouse_y -= dy;  // Invert Y axis

    // Clamp to screen boundaries
    if (mouse_x < 0) mouse_x = 0;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_x >= fb_width - 16) mouse_x = fb_width - 16;
    if (mouse_y >= fb_height - 16) mouse_y = fb_height - 16;

    // In a real implementation, you would update the display here
    // For now, we'll just update the cursor position
    #ifdef DEBUG_MOUSE
    graphics_clear(framebuffer, fb_width, fb_height, 0x0000FF);
    draw_string("Welcome to MyOS", 50, 50, 0xFFFFFF);
    #endif
    mouse_draw_at(framebuffer, fb_width, mouse_x, mouse_y);
}

void mouse_draw_at(uint32_t *fb, int width, int x, int y) {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (mouse_cursor[j][i] == 1) {
                // Black outline
                fb[(y + j) * width + (x + i)] = 0x000000;
            } else if (mouse_cursor[j][i] == 2) {
                // White fill
                fb[(y + j) * width + (x + i)] = 0xFFFFFF;
            }
        }
    }
}

// Simple implementation of mouse_draw using mouse_draw_at
void mouse_draw(uint32_t *fb, int width) {
    mouse_draw_at(fb, width, mouse_x, mouse_y);
}

// Get current mouse position
void mouse_get_position(int *x, int *y) {
    if (x) *x = mouse_x;
    if (y) *y = mouse_y;
}

// Set mouse position
void mouse_set_position(int x, int y) {
    mouse_x = x;
    mouse_y = y;
    // Clamp to screen boundaries
    if (mouse_x < 0) mouse_x = 0;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_x >= fb_width - 16) mouse_x = fb_width - 16;
    if (mouse_y >= fb_height - 16) mouse_y = fb_height - 16;
}