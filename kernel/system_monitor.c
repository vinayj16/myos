#include "system_monitor.h"
#include "theme.h"
#include "text.h"
#include "framebuffer.h"
#include "mm/mm.h"
#include "process.h"
#include "libc/string.h"
#include "libc/stdio.h"

static memory_stats_t memory_stats;
static cpu_stats_t cpu_stats;
static system_info_t system_info;
static uint32_t update_counter = 0;

void system_monitor_init(void) {
    memset(&memory_stats, 0, sizeof(memory_stats));
    memset(&cpu_stats, 0, sizeof(cpu_stats));
    memset(&system_info, 0, sizeof(system_info));
    
    // Initialize system info
    strcpy(system_info.kernel_version, "MyOS v1.0.0");
    strcpy(system_info.system_name, "MyOS Desktop");
    system_info.boot_time = 0; // Would be set during boot
    
    update_counter = 0;
}

void system_monitor_update(void) {
    update_counter++;
    
    // Update memory stats (get from memory manager)
    // This would interface with the actual memory manager
    memory_stats.total_memory = 64 * 1024 * 1024; // 64MB example
    memory_stats.used_memory = 16 * 1024 * 1024;  // 16MB example
    memory_stats.free_memory = memory_stats.total_memory - memory_stats.used_memory;
    memory_stats.cached_memory = 4 * 1024 * 1024; // 4MB example
    
    // Update CPU stats (simulated for now)
    cpu_stats.cpu_usage_percent = (update_counter * 7) % 100; // Simulated varying load
    cpu_stats.processes_running = 3; // Example
    cpu_stats.processes_total = 5;   // Example
    cpu_stats.context_switches = update_counter * 10;
    
    // Update system uptime
    system_info.uptime_seconds = update_counter; // Simplified
}

memory_stats_t system_monitor_get_memory_stats(void) {
    return memory_stats;
}

cpu_stats_t system_monitor_get_cpu_stats(void) {
    return cpu_stats;
}

system_info_t system_monitor_get_system_info(void) {
    return system_info;
}

int system_monitor_get_processes(process_info_t* processes, int max_processes) {
    // This would interface with the actual process manager
    // For now, return some example processes
    if (max_processes < 1) return 0;
    
    // Example processes
    strcpy(processes[0].name, "kernel");
    processes[0].pid = 0;
    processes[0].memory_usage = 8 * 1024 * 1024; // 8MB
    processes[0].cpu_usage = 5;
    strcpy(processes[0].state, "running");
    
    if (max_processes < 2) return 1;
    
    strcpy(processes[1].name, "desktop");
    processes[1].pid = 1;
    processes[1].memory_usage = 4 * 1024 * 1024; // 4MB
    processes[1].cpu_usage = 15;
    strcpy(processes[1].state, "running");
    
    if (max_processes < 3) return 2;
    
    strcpy(processes[2].name, "calculator");
    processes[2].pid = 2;
    processes[2].memory_usage = 1 * 1024 * 1024; // 1MB
    processes[2].cpu_usage = 2;
    strcpy(processes[2].state, "sleeping");
    
    return 3; // Return number of processes
}

void system_monitor_draw_window(int x, int y, int width, int height) {
    // Draw window background
    fb_draw_rect(x, y, width, height, current_theme.window_background);
    
    // Draw sections
    int section_height = (height - 40) / 3;
    int current_y = y + 30;
    
    // Memory section
    draw_string("Memory Usage:", x + 10, current_y, current_theme.text_primary);
    current_y += 20;
    
    char mem_text[128];
    snprintf(mem_text, sizeof(mem_text), "Total: %u MB", memory_stats.total_memory / (1024 * 1024));
    draw_string(mem_text, x + 20, current_y, current_theme.text_secondary);
    current_y += 16;
    
    snprintf(mem_text, sizeof(mem_text), "Used: %u MB", memory_stats.used_memory / (1024 * 1024));
    draw_string(mem_text, x + 20, current_y, current_theme.text_secondary);
    current_y += 16;
    
    snprintf(mem_text, sizeof(mem_text), "Free: %u MB", memory_stats.free_memory / (1024 * 1024));
    draw_string(mem_text, x + 20, current_y, current_theme.text_secondary);
    current_y += 30;
    
    // CPU section
    draw_string("CPU Information:", x + 10, current_y, current_theme.text_primary);
    current_y += 20;
    
    char cpu_text[128];
    snprintf(cpu_text, sizeof(cpu_text), "Usage: %u%%", cpu_stats.cpu_usage_percent);
    draw_string(cpu_text, x + 20, current_y, current_theme.text_secondary);
    current_y += 16;
    
    snprintf(cpu_text, sizeof(cpu_text), "Processes: %u running, %u total", 
             cpu_stats.processes_running, cpu_stats.processes_total);
    draw_string(cpu_text, x + 20, current_y, current_theme.text_secondary);
    current_y += 30;
    
    // System info section
    draw_string("System Information:", x + 10, current_y, current_theme.text_primary);
    current_y += 20;
    
    draw_string(system_info.kernel_version, x + 20, current_y, current_theme.text_secondary);
    current_y += 16;
    
    char uptime_text[64];
    snprintf(uptime_text, sizeof(uptime_text), "Uptime: %u seconds", system_info.uptime_seconds);
    draw_string(uptime_text, x + 20, current_y, current_theme.text_secondary);
    
    // Draw CPU usage bar
    int bar_x = x + width - 150;
    int bar_y = y + 50;
    int bar_width = 100;
    int bar_height = 20;
    
    // Background
    fb_draw_rect(bar_x, bar_y, bar_width, bar_height, 0x333333);
    
    // Usage bar
    int usage_width = (bar_width * cpu_stats.cpu_usage_percent) / 100;
    uint32_t usage_color = cpu_stats.cpu_usage_percent > 80 ? 0xFF4444 : 
                          cpu_stats.cpu_usage_percent > 50 ? 0xFFAA44 : 0x44FF44;
    fb_draw_rect(bar_x, bar_y, usage_width, bar_height, usage_color);
    
    // Border
    fb_draw_rect(bar_x, bar_y, bar_width, 2, current_theme.window_border);
    fb_draw_rect(bar_x, bar_y + bar_height - 2, bar_width, 2, current_theme.window_border);
    fb_draw_rect(bar_x, bar_y, 2, bar_height, current_theme.window_border);
    fb_draw_rect(bar_x + bar_width - 2, bar_y, 2, bar_height, current_theme.window_border);
}

void system_monitor_handle_click(int x, int y, int window_x, int window_y) {
    (void)x; (void)y; (void)window_x; (void)window_y;
    // Handle clicks in system monitor window
    // Could implement process killing, sorting, etc.
}
