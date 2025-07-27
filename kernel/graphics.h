#pragma once
#include <stdint.h>

#define VGA_WIDTH  320
#define VGA_HEIGHT 200

void vga_clear(uint8_t color);
void vga_putpixel(int x, int y, uint8_t color);
