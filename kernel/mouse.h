#ifndef _MOUSE_H
#define _MOUSE_H

#include <stdint.h>

// Function declarations
void mouse_init(uint32_t *fb, int width, int height);
void mouse_enable(void);
void mouse_disable(void);
void mouse_set_position(int x, int y);
void mouse_get_position(int *x, int *y);
void mouse_process_packet(int8_t dx, int8_t dy);
void mouse_draw(uint32_t *fb, int width);
void mouse_draw_at(uint32_t *fb, int width, int x, int y);
uint8_t mouse_read(void);

#endif // _MOUSE_H