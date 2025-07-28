#ifndef _SYSTEM_MONITOR_H
#define _SYSTEM_MONITOR_H

#include <stdint.h>

typedef struct {
    uint32_t total_memory;
    uint32_t used_memory;
    uint32_t free_memory;
    uint32_t cached_memory;
} memory_stats_t;

typedef struct {
    uint32_t cpu_usage_percent;
    uint32_t processes_running;
    uint32_t processes_total;
    uint32_t context_switches;
} cpu_stats_t;

typedef struct {
    uint32_t uptime_seconds;
    uint32_t boot_time;
    char kernel_version[32];
    char system_name[32];
} system_info_t;

typedef struct {
    uint32_t pid;
    char name[32];
    uint32_t memory_usage;
    uint32_t cpu_usage;
    char state[16];
} process_info_t;

// System monitoring functions
void system_monitor_init(void);
void system_monitor_update(void);
memory_stats_t system_monitor_get_memory_stats(void);
cpu_stats_t system_monitor_get_cpu_stats(void);
system_info_t system_monitor_get_system_info(void);
int system_monitor_get_processes(process_info_t* processes, int max_processes);

// System monitor window
void system_monitor_draw_window(int x, int y, int width, int height);
void system_monitor_handle_click(int x, int y, int window_x, int window_y);

#endif // _SYSTEM_MONITOR_H
