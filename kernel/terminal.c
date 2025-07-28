#include "terminal.h"
#include "theme.h"
#include "font.h"
#include "framebuffer.h"
#include "system_monitor.h"
#include "libc/string.h"
#include "libc/stdio.h"
#include <stdarg.h>

void terminal_init(terminal_t* term, int x, int y, int width, int height) {
    memset(term, 0, sizeof(terminal_t));
    
    term->window_x = x;
    term->window_y = y;
    term->window_width = width;
    term->window_height = height;
    
    term->cursor_x = 0;
    term->cursor_y = 0;
    term->scroll_offset = 0;
    term->current_fg = 0x00FF00; // Green text (classic terminal)
    term->current_bg = 0x000000; // Black background
    term->current_attributes = 0;
    term->history_count = 0;
    term->history_position = 0;
    term->input_length = 0;
    
    // Clear the buffer
    for (int i = 0; i < TERMINAL_BUFFER_SIZE; i++) {
        term->buffer[i].character = ' ';
        term->buffer[i].foreground = term->current_fg;
        term->buffer[i].background = term->current_bg;
        term->buffer[i].attributes = 0;
    }
    
    // Show welcome message
    terminal_puts(term, "MyOS Terminal v1.0\n");
    terminal_puts(term, "Type 'help' for available commands.\n");
    terminal_puts(term, "$ ");
}

void terminal_clear(terminal_t* term) {
    for (int i = 0; i < TERMINAL_BUFFER_SIZE; i++) {
        term->buffer[i].character = ' ';
        term->buffer[i].foreground = term->current_fg;
        term->buffer[i].background = term->current_bg;
        term->buffer[i].attributes = 0;
    }
    term->cursor_x = 0;
    term->cursor_y = 0;
    term->scroll_offset = 0;
}

void terminal_putchar(terminal_t* term, char c) {
    if (c == '\n') {
        term->cursor_x = 0;
        term->cursor_y++;
        if (term->cursor_y >= TERMINAL_HEIGHT) {
            terminal_scroll_up(term);
            term->cursor_y = TERMINAL_HEIGHT - 1;
        }
        return;
    }
    
    if (c == '\r') {
        term->cursor_x = 0;
        return;
    }
    
    if (c == '\b') {
        if (term->cursor_x > 0) {
            term->cursor_x--;
            int pos = term->cursor_y * TERMINAL_WIDTH + term->cursor_x;
            term->buffer[pos].character = ' ';
        }
        return;
    }
    
    if (c == '\t') {
        // Tab to next 8-character boundary
        int spaces = 8 - (term->cursor_x % 8);
        for (int i = 0; i < spaces; i++) {
            terminal_putchar(term, ' ');
        }
        return;
    }
    
    // Regular character
    if (term->cursor_x >= TERMINAL_WIDTH) {
        terminal_putchar(term, '\n');
    }
    
    int pos = term->cursor_y * TERMINAL_WIDTH + term->cursor_x;
    term->buffer[pos].character = c;
    term->buffer[pos].foreground = term->current_fg;
    term->buffer[pos].background = term->current_bg;
    term->buffer[pos].attributes = term->current_attributes;
    
    term->cursor_x++;
}

void terminal_puts(terminal_t* term, const char* str) {
    while (*str) {
        terminal_putchar(term, *str++);
    }
}

void terminal_printf(terminal_t* term, const char* format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    terminal_puts(term, buffer);
}

void terminal_handle_key(terminal_t* term, char key) {
    if (key == '\n' || key == '\r') {
        // Execute command
        term->input_buffer[term->input_length] = '\0';
        terminal_putchar(term, '\n');
        
        if (term->input_length > 0) {
            // Add to history
            if (term->history_count < TERMINAL_HISTORY_SIZE) {
                strcpy(term->history[term->history_count], term->input_buffer);
                term->history_count++;
            }
            
            // Execute command
            terminal_execute_command(term, term->input_buffer);
        }
        
        // Reset input
        term->input_length = 0;
        terminal_puts(term, "$ ");
        
    } else if (key == '\b') {
        // Backspace
        if (term->input_length > 0) {
            term->input_length--;
            terminal_putchar(term, '\b');
        }
        
    } else if (key >= 32 && key <= 126) {
        // Printable character
        if (term->input_length < sizeof(term->input_buffer) - 1) {
            term->input_buffer[term->input_length++] = key;
            terminal_putchar(term, key);
        }
    }
}

void terminal_scroll_up(terminal_t* term) {
    // Move all lines up by one
    for (int y = 0; y < TERMINAL_HEIGHT - 1; y++) {
        for (int x = 0; x < TERMINAL_WIDTH; x++) {
            int src = (y + 1) * TERMINAL_WIDTH + x;
            int dst = y * TERMINAL_WIDTH + x;
            term->buffer[dst] = term->buffer[src];
        }
    }
    
    // Clear the last line
    for (int x = 0; x < TERMINAL_WIDTH; x++) {
        int pos = (TERMINAL_HEIGHT - 1) * TERMINAL_WIDTH + x;
        term->buffer[pos].character = ' ';
        term->buffer[pos].foreground = term->current_fg;
        term->buffer[pos].background = term->current_bg;
        term->buffer[pos].attributes = 0;
    }
}

void terminal_scroll_down(terminal_t* term) {
    // Move all lines down by one
    for (int y = TERMINAL_HEIGHT - 1; y > 0; y--) {
        for (int x = 0; x < TERMINAL_WIDTH; x++) {
            int src = (y - 1) * TERMINAL_WIDTH + x;
            int dst = y * TERMINAL_WIDTH + x;
            term->buffer[dst] = term->buffer[src];
        }
    }
    
    // Clear the first line
    for (int x = 0; x < TERMINAL_WIDTH; x++) {
        int pos = x;
        term->buffer[pos].character = ' ';
        term->buffer[pos].foreground = term->current_fg;
        term->buffer[pos].background = term->current_bg;
        term->buffer[pos].attributes = 0;
    }
}

void terminal_draw(terminal_t* term, uint32_t* framebuffer, uint32_t fb_width) {
    (void)framebuffer; (void)fb_width; // Use global framebuffer
    
    // Draw terminal background
    fb_draw_rect(term->window_x, term->window_y, term->window_width, term->window_height, 0x000000);
    
    // Draw characters
    for (int y = 0; y < TERMINAL_HEIGHT; y++) {
        for (int x = 0; x < TERMINAL_WIDTH; x++) {
            int pos = y * TERMINAL_WIDTH + x;
            terminal_cell_t* cell = &term->buffer[pos];
            
            if (cell->character != ' ') {
                int pixel_x = term->window_x + x * FONT_WIDTH;
                int pixel_y = term->window_y + y * FONT_HEIGHT;
                
                // Draw character background if different from terminal background
                if (cell->background != 0x000000) {
                    fb_draw_rect(pixel_x, pixel_y, FONT_WIDTH, FONT_HEIGHT, cell->background);
                }
                
                // Draw character
                draw_char(cell->character, pixel_x, pixel_y, cell->foreground);
            }
        }
    }
    
    // Draw cursor
    int cursor_pixel_x = term->window_x + term->cursor_x * FONT_WIDTH;
    int cursor_pixel_y = term->window_y + term->cursor_y * FONT_HEIGHT;
    fb_draw_rect(cursor_pixel_x, cursor_pixel_y, FONT_WIDTH, 2, term->current_fg);
}

void terminal_execute_command(terminal_t* term, const char* command) {
    // Parse command and arguments
    char cmd[64];
    char args[192];
    int i = 0;
    
    // Extract command
    while (command[i] && command[i] != ' ' && i < sizeof(cmd) - 1) {
        cmd[i] = command[i];
        i++;
    }
    cmd[i] = '\0';
    
    // Skip spaces
    while (command[i] == ' ') i++;
    
    // Extract arguments
    strcpy(args, &command[i]);
    
    // Execute built-in commands
    if (strcmp(cmd, "help") == 0) {
        terminal_cmd_help(term);
    } else if (strcmp(cmd, "clear") == 0) {
        terminal_cmd_clear(term);
    } else if (strcmp(cmd, "echo") == 0) {
        terminal_cmd_echo(term, args);
    } else if (strcmp(cmd, "ps") == 0) {
        terminal_cmd_ps(term);
    } else if (strcmp(cmd, "mem") == 0) {
        terminal_cmd_mem(term);
    } else if (strcmp(cmd, "uptime") == 0) {
        terminal_cmd_uptime(term);
    } else if (strcmp(cmd, "version") == 0) {
        terminal_cmd_version(term);
    } else if (strlen(cmd) == 0) {
        // Empty command, do nothing
    } else {
        terminal_printf(term, "Command not found: %s\n", cmd);
        terminal_puts(term, "Type 'help' for available commands.\n");
    }
}

void terminal_cmd_help(terminal_t* term) {
    terminal_puts(term, "Available commands:\n");
    terminal_puts(term, "  help     - Show this help message\n");
    terminal_puts(term, "  clear    - Clear the terminal screen\n");
    terminal_puts(term, "  echo     - Display a line of text\n");
    terminal_puts(term, "  ps       - Show running processes\n");
    terminal_puts(term, "  mem      - Show memory usage\n");
    terminal_puts(term, "  uptime   - Show system uptime\n");
    terminal_puts(term, "  version  - Show system version\n");
}

void terminal_cmd_clear(terminal_t* term) {
    terminal_clear(term);
}

void terminal_cmd_echo(terminal_t* term, const char* args) {
    terminal_puts(term, args);
    terminal_puts(term, "\n");
}

void terminal_cmd_ps(terminal_t* term) {
    terminal_puts(term, "PID  NAME         STATE    MEM\n");
    terminal_puts(term, "---  -----------  -------  -------\n");
    
    process_info_t processes[10];
    int count = system_monitor_get_processes(processes, 10);
    
    for (int i = 0; i < count; i++) {
        terminal_printf(term, "%3u  %-11s  %-7s  %4uMB\n",
                       processes[i].pid,
                       processes[i].name,
                       processes[i].state,
                       processes[i].memory_usage / (1024 * 1024));
    }
}

void terminal_cmd_mem(terminal_t* term) {
    memory_stats_t stats = system_monitor_get_memory_stats();
    
    terminal_printf(term, "Memory Usage:\n");
    terminal_printf(term, "  Total: %u MB\n", stats.total_memory / (1024 * 1024));
    terminal_printf(term, "  Used:  %u MB\n", stats.used_memory / (1024 * 1024));
    terminal_printf(term, "  Free:  %u MB\n", stats.free_memory / (1024 * 1024));
    terminal_printf(term, "  Cache: %u MB\n", stats.cached_memory / (1024 * 1024));
}

void terminal_cmd_uptime(terminal_t* term) {
    system_info_t info = system_monitor_get_system_info();
    
    uint32_t hours = info.uptime_seconds / 3600;
    uint32_t minutes = (info.uptime_seconds % 3600) / 60;
    uint32_t seconds = info.uptime_seconds % 60;
    
    terminal_printf(term, "System uptime: %u:%02u:%02u\n", hours, minutes, seconds);
}

void terminal_cmd_version(terminal_t* term) {
    system_info_t info = system_monitor_get_system_info();
    terminal_printf(term, "%s\n", info.kernel_version);
    terminal_puts(term, "Built with advanced features:\n");
    terminal_puts(term, "- 64-bit kernel architecture\n");
    terminal_puts(term, "- Graphical desktop environment\n");
    terminal_puts(term, "- Memory management\n");
    terminal_puts(term, "- Process scheduling\n");
    terminal_puts(term, "- Virtual filesystem\n");
}
