#include <stdint.h>
#include "graphics.h"

static uint8_t* const VGA = (uint8_t*)0xA0000; // Mode 13h framebuffer

void vga_clear(uint8_t color) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA[i] = color;
    }
}

void vga_putpixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) return;
    VGA[y * VGA_WIDTH + x] = color;
}