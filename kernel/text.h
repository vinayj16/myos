#pragma once
#include <stdint.h>

void text_set_framebuffer(uint32_t *framebuffer, uint32_t width, uint32_t height);
void draw_char(char ch, int x, int y, uint32_t color);
void draw_string(const char *s, int x, int y, uint32_t color);
void text_putchar(char c);
void text_clear_screen(void);
