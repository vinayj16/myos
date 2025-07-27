#include "framebuffer.h"
#include <stddef.h>
#include <string.h>

// VGA Mode 13h is 320x200 with 8-bit color
#define VGA_WIDTH 320
#define VGA_HEIGHT 200

// Framebuffer state - we'll use the actual VGA memory
// instead of our own buffer to save memory
static uint8_t *vga_buffer = (uint8_t*)0xA0000;

// Global variables for external use
uint32_t *framebuffer = (uint32_t*)0xA0000; // Direct mapping to VGA memory
uint32_t fb_width = VGA_WIDTH;
uint32_t fb_height = VGA_HEIGHT;
uint32_t fb_pitch = VGA_WIDTH; // 1 byte per pixel in mode 13h

// Simple font for text rendering
static const uint8_t font8x8_basic[128][8] = {
    // Basic ASCII characters (0-127)
    // Each character is 8x8 pixels
    // Format: 8 bytes per character, each byte is a row of pixels (LSB is leftmost pixel)
    [0 ... 127] = {0}, // Initialize all to zero
    
    // Override specific characters as needed
    ['A'] = {0x18, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00},
    // Add more characters as needed
};

// VGA color palette is now in framebuffer.h as inline function

// Initialize the framebuffer
void init_graphics(uint32_t *fb, uint32_t width, uint32_t height) {
    // Update global framebuffer variables
    framebuffer = fb;
    fb_width = width;
    fb_height = height;
    fb_pitch = width * 4; // Assuming 32-bit pixels
    // Set video mode to 13h (320x200, 256 colors)
    __asm__ volatile(
        "mov $0x13, %%ah\n"
        "int $0x10"
        : : : "eax", "memory"
    );
    
    // Clear the screen
    fb_clear(0);
    
    // VGA palette is already set up by the BIOS in mode 13h
    // We use the standard 16-color palette for compatibility
}

// Clear the screen with a color
void fb_clear(uint32_t color) {
    // Convert ARGB to VGA color index
    uint8_t vga_color = argb_to_vga(color);
    
    // Clear the VGA buffer
    __asm__ volatile(
        "cld\n"
        "mov %0, %%al\n"
        "mov $0xA0000, %%edi\n"
        "mov $0xFA00, %%ecx\n"  // 320*200/2 = 32000 words
        "rep stosw\n"
        : 
        : "r"(vga_color)
        : "eax", "ecx", "edi", "memory"
    );
}

// Draw a single pixel
void fb_draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || y < 0 || x >= (int)fb_width || y >= (int)fb_height) {
        return;
    }
    
    // Directly write to VGA memory
    vga_buffer[y * fb_width + x] = argb_to_vga(color);
}

// Draw a rectangle
void fb_draw_rect(int x, int y, int width, int height, uint32_t color) {
    // Clip the rectangle to the framebuffer bounds
    if (x >= (int)fb_width || y >= (int)fb_height) return;
    if (x < 0) { width += x; x = 0; }
    if (y < 0) { height += y; y = 0; }
    if (x + width > (int)fb_width) width = fb_width - x;
    if (y + height > (int)fb_height) height = fb_height - y;
    if (width <= 0 || height <= 0) return;
    
    uint8_t vga_color = argb_to_vga(color);
    
    // Draw the rectangle
    for (int dy = 0; dy < height; dy++) {
        uint8_t *row = vga_buffer + (y + dy) * fb_width + x;
        for (int dx = 0; dx < width; dx++) {
            *row++ = vga_color;
        }
    }
}

// Draw a rectangle with rounded corners
void fb_draw_rounded_rect(int x, int y, int width, int height, int radius, uint32_t color) {
    (void)radius; // Unused for now
    // For now, just draw a regular rectangle
    fb_draw_rect(x, y, width, height, color);
}

// Font rendering is handled by the font.h system

// Draw a string (simple implementation)
void fb_draw_string(const char *str, int x, int y, uint32_t color) {
    if (!str) return;
    
    // Convert color to VGA index
    uint8_t vga_color = argb_to_vga(color);
    
    // Draw each character
    for (int i = 0; str[i] != '\0'; i++) {
        uint8_t c = (uint8_t)str[i];
        
        // Skip non-printable characters
        if (c < 32 || c > 127) {
            x += 8; // Advance cursor
            continue;
        }
        
        // Draw character at position
        for (int dy = 0; dy < 8; dy++) {
            for (int dx = 0; dx < 8; dx++) {
                if (font8x8_basic[c][dy] & (1 << (7 - dx))) {
                    int px = x + dx;
                    int py = y + dy;
                    
                    // Check bounds
                    if (px >= 0 && px < VGA_WIDTH && py >= 0 && py < VGA_HEIGHT) {
                        vga_buffer[py * VGA_WIDTH + px] = vga_color;
                    }
                }
            }
        }
        
        // Move to next character position
        x += 8;
    }
}
