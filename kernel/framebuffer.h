#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <stdint.h>
#include <stddef.h>

// VGA Mode 13h dimensions
#define VGA_WIDTH 320
#define VGA_HEIGHT 200

// Framebuffer information - these are defined in framebuffer.c
extern uint32_t *framebuffer;  // Direct mapping to VGA memory
extern uint32_t fb_width;      // 320 pixels
extern uint32_t fb_height;     // 200 pixels
extern uint32_t fb_pitch;      // 320 bytes (1 byte per pixel)

// Function declarations
void init_graphics(uint32_t *fb, uint32_t width, uint32_t height);
void fb_clear(uint32_t color);
void fb_draw_pixel(int x, int y, uint32_t color);
void fb_draw_rect(int x, int y, int width, int height, uint32_t color);
void fb_draw_rounded_rect(int x, int y, int width, int height, int radius, uint32_t color);
void fb_draw_string(const char *str, int x, int y, uint32_t color);

// Convert 32-bit ARGB color to VGA color index (16 colors)
static inline uint8_t argb_to_vga(uint32_t color) {
    // Extract RGB components
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    
    // Simple RGB to VGA color mapping (16 colors)
    // This is a basic mapping - you might want to implement a more sophisticated one
    uint8_t vga_color = 0;
    
    // Check for black
    if (r < 32 && g < 32 && b < 32) return 0;
    
    // Check for white
    if (r > 192 && g > 192 && b > 192) return 0x0F;
    
    // Check for grays
    if (r == g && g == b) {
        if (r > 128) return 0x07; // Light gray
        return 0x08; // Dark gray
    }
    
    // Basic colors
    if (r > 128) vga_color |= 0x04; // Red
    if (g > 128) vga_color |= 0x02; // Green
    if (b > 128) vga_color |= 0x01; // Blue
    
    // Make bright colors more visible
    if (r > 200 || g > 200 || b > 200) {
        vga_color |= 0x08; // High intensity
    }
    
    return vga_color;
}

// Convert VGA color index to ARGB (for compatibility)
static inline uint32_t vga_to_argb(uint8_t color) {
    // Simple VGA to RGB mapping (16 colors)
    static const uint32_t vga_colors[16] = {
        0xFF000000, // 0: Black
        0xFF0000AA, // 1: Blue
        0xFF00AA00, // 2: Green
        0xFF00AAAA, // 3: Cyan
        0xFFAA0000, // 4: Red
        0xFFAA00AA, // 5: Magenta
        0xFFAA5500, // 6: Brown
        0xFFAAAAAA, // 7: Light Gray
        0xFF555555, // 8: Dark Gray
        0xFF5555FF, // 9: Light Blue
        0xFF55FF55, // 10: Light Green
        0xFF55FFFF, // 11: Light Cyan
        0xFFFF5555, // 12: Light Red
        0xFFFF55FF, // 13: Light Magenta
        0xFFFFFF55, // 14: Yellow
        0xFFFFFFFF  // 15: White
    };
    
    return vga_colors[color & 0x0F];
}

#endif // _FRAMEBUFFER_H
