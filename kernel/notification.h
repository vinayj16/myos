#ifndef _NOTIFICATION_H
#define _NOTIFICATION_H

#include <stdint.h>

#define MAX_NOTIFICATIONS 10
#define NOTIFICATION_TIMEOUT 5000  // 5 seconds in milliseconds
#define NOTIFICATION_WIDTH 300
#define NOTIFICATION_HEIGHT 80

typedef enum {
    NOTIFICATION_INFO,
    NOTIFICATION_WARNING,
    NOTIFICATION_ERROR,
    NOTIFICATION_SUCCESS
} notification_type_t;

typedef struct {
    char title[64];
    char message[128];
    notification_type_t type;
    uint32_t timestamp;
    int active;
    int x, y;
} notification_t;

// Notification system functions
void notification_init(void);
void notification_show(const char* title, const char* message, notification_type_t type);
void notification_update(uint32_t current_time);
void notification_draw(uint32_t* framebuffer, uint32_t width, uint32_t height);
void notification_clear_all(void);

// Convenience functions
void notify_info(const char* title, const char* message);
void notify_warning(const char* title, const char* message);
void notify_error(const char* title, const char* message);
void notify_success(const char* title, const char* message);

#endif // _NOTIFICATION_H