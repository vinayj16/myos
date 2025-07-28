#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <stdint.h>

#define TERMINAL_WIDTH 80
#define TERMINAL_HEIGHT 25
#define TERMINAL_BUFFER_SIZE (TERMINAL_WIDTH * TERMINAL_HEIGHT)
#define TERMINAL_HISTORY_SIZE 1000

typedef struct {
    char character;
    uint32_t foreground;
    uint32_t background;
    uint8_t attributes; // Bold, italic, underline, etc.
} terminal_cell_t;

typedef struct {
    terminal_cell_t buffer[TERMINAL_BUFFER_SIZE];
    char history[TERMINAL_HISTORY_SIZE][TERMINAL_WIDTH];
    int history_count;
    int history_position;
    int cursor_x;
    int cursor_y;
    int scroll_offset;
    uint32_t current_fg;
    uint32_t current_bg;
    uint8_t current_attributes;
    char input_buffer[256];
    int input_length;
    int window_x, window_y;
    int window_width, window_height;
} terminal_t;

// Terminal functions
void terminal_init(terminal_t* term, int x, int y, int width, int height);
void terminal_clear(terminal_t* term);
void terminal_putchar(terminal_t* term, char c);
void terminal_puts(terminal_t* term, const char* str);
void terminal_printf(terminal_t* term, const char* format, ...);
void terminal_handle_key(terminal_t* term, char key);
void terminal_draw(terminal_t* term, uint32_t* framebuffer, uint32_t fb_width);
void terminal_scroll_up(terminal_t* term);
void terminal_scroll_down(terminal_t* term);
void terminal_execute_command(terminal_t* term, const char* command);

// Built-in commands
void terminal_cmd_help(terminal_t* term);
void terminal_cmd_clear(terminal_t* term);
void terminal_cmd_echo(terminal_t* term, const char* args);
void terminal_cmd_ps(terminal_t* term);
void terminal_cmd_mem(terminal_t* term);
void terminal_cmd_uptime(terminal_t* term);
void terminal_cmd_version(terminal_t* term);

#endif // _TERMINAL_H
