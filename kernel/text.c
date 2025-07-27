#include <stdint.h>
#include "font.h"

static uint32_t *fb;
static uint32_t fb_width;
static uint32_t fb_height;

void text_set_framebuffer(uint32_t *framebuffer, uint32_t width, uint32_t height) {
    fb = framebuffer;
    fb_width = width;
    fb_height = height;
}

void draw_char(char ch, int x, int y, uint32_t color) {
    if (!fb) return;

    for (int row = 0; row < FONT_HEIGHT; row++) {
        uint8_t bits = font8x16[(int)ch][row];
        for (int col = 0; col < FONT_WIDTH; col++) {
            if (bits & (1 << (7 - col))) {
                fb[(y + row) * fb_width + (x + col)] = color;
            }
        }
    }
}

void draw_string(const char *s, int x, int y, uint32_t color) {
    while (*s) {
        draw_char(*s, x, y, color);
        x += FONT_WIDTH;
        s++;
    }
}
// Simple text output for printf
static int cursor_x = 0;
static int cursor_y = 0;

void text_putchar(char c) {
    if (!fb) return;
    
    if (c == '\n') {
        cursor_x = 0;
        cursor_y += FONT_HEIGHT;
        if (cursor_y >= (int)fb_height) {
            cursor_y = 0; // Wrap to top
        }
        return;
    }
    
    if (c == '\r') {
        cursor_x = 0;
        return;
    }
    
    draw_char(c, cursor_x, cursor_y, 0xFFFFFF); // White text
    cursor_x += FONT_WIDTH;
    
    if (cursor_x >= (int)fb_width) {
        cursor_x = 0;
        cursor_y += FONT_HEIGHT;
        if (cursor_y >= (int)fb_height) {
            cursor_y = 0; // Wrap to top
        }
    }
}

void text_clear_screen(void) {
    cursor_x = 0;
    cursor_y = 0;
    if (fb) {
        for (uint32_t i = 0; i < fb_width * fb_height; i++) {
            fb[i] = 0x000000; // Black background
        }
    }
}