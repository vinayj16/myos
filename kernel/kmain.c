#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "text.h"
#include "font.h"
#include "graphics.h"
#include "mouse.h"
#include "keyboard.h"
#include "window.h"
#include "desktop.h"
#include "idt.h"
#include "mm/mm.h"
#include "process.h"
#include "fs.h"
#include "ui.h"

// Forward declarations
void init_graphics(uint32_t *framebuffer, uint32_t width, uint32_t height);
void draw_string(const char *str, int x, int y, uint32_t color);

// Add extern declarations for IRQ stubs
extern void irq0_stub(void);
extern void irq1_stub(void);
extern void irq12_stub(void);

struct multiboot_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot_tag_basic_meminfo {
    uint32_t type;
    uint32_t size;
    uint32_t mem_lower;
    uint32_t mem_upper;
};

struct multiboot_tag_framebuffer {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint16_t reserved;
};

void mouse_irq_handler(void) {
    static uint8_t packet[3];
    static int packet_index = 0;
    uint8_t data = mouse_read();
    packet[packet_index++] = data;
    if (packet_index == 3) {
        int8_t dx = packet[1];
        int8_t dy = packet[2];
        mouse_process_packet(dx, dy);
        packet_index = 0;
    }
}

// The kernel entry point called from boot.s
// Use extern declarations from framebuffer.h
// Add this near the top of the file
void debug_print(const char* message, uint32_t length);

// In the kernel_main function, add this as the first line:
debug_print("Kernel starting...\n", 18);

void kernel_main(void) {
    // Early debug output
    printf("MyOS Kernel Starting...\n");
    
    // For now, use default values since we're in simple 32-bit mode
    uint32_t mem_upper = 64 * 1024; // Default to 64MB
    
    printf("Initializing in 32-bit mode...\n");

    // Set up default framebuffer for VGA mode
    framebuffer = (uint32_t *)0xA0000; // VGA memory
    fb_width = 320;
    fb_height = 200;
    
    printf("Using VGA mode: %dx%d at 0x%p\n", fb_width, fb_height, framebuffer);
    
    // Initialize graphics
    init_graphics(framebuffer, fb_width, fb_height);
    printf("Graphics initialized.\n");
    
    // Initialize memory manager
    printf("Initializing memory manager...\n");
    if (mem_upper > 0) {
        mm_init(mem_upper);
    } else {
        // Default to 64MB if we couldn't detect memory
        mm_init(64 * 1024);
    }
    printf("Memory manager initialized.\n");
    
    // Initialize text system
    text_set_framebuffer(framebuffer, fb_width, fb_height);

    // Initialize IDT and PIC
    idt_init();
    pic_remap();
    idt_set_gate(32+12, (uint64_t)irq12_stub, 0x08, 0x8E); // IRQ12
    irq_install_handler(12, mouse_irq_handler);

    // Initialize process management
    process_init();
    
    // Initialize filesystem
    fs_init();
    
    // Initialize UI system
    ui_init(framebuffer, fb_width, fb_height);
    
    // Create a test process
    void test_process(void) {
        while (1) {
            // Process system messages
            process_sleep(100);
        }
    }
    process_create(test_process, 1);

    // Initialize desktop environment
    printf("Starting desktop environment...\n");
    desktop_init(framebuffer, fb_width, fb_height);
    printf("Desktop initialized. Entering main loop...\n");

    // Main system loop
    // Removed unused variable
    uint32_t frame_count = 0;
    
    for(;;) {
        // Handle input
        desktop_handle_input();
        
        // Update UI
        ui_update();
        
        // Simple frame limiter and status update
        if (frame_count++ % 30 == 0) {  // Update status every 30 frames
            char status[128];
            // Get memory stats from MM
            uint32_t mem_used = 0;  // This should be replaced with actual memory usage
            uint32_t current_pid = 0;  // This should be replaced with current process ID
            
            snprintf(status, sizeof(status), "Memory: %u KB | Process: %u", 
                    mem_used / 1024, 
                    current_pid);
            
            // Draw status in top-right corner
            uint32_t status_x = fb_width - (strlen(status) * 8) - 20;
            draw_string(status, status_x, 10, 0xFFFFFF);
        }
        
        // Simple delay to prevent excessive CPU usage
        for (volatile int i = 0; i < 100000; i++) {
            __asm__ volatile("nop");
        }
        
        // Yield to other processes
        process_yield();
    }
}
