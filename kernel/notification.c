#include "notification.h"
#include "theme.h"
#include "text.h"
#include "framebuffer.h"
#include "libc/string.h"
#include "libc/stdio.h"

static notification_t notifications[MAX_NOTIFICATIONS];
static int notification_count = 0;

void notification_init(void) {
    memset(notifications, 0, sizeof(notifications));
    notification_count = 0;
}

void notification_show(const char* title, const char* message, notification_type_t type) {
    // Find an empty slot or reuse the oldest one
    int slot = -1;
    for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
        if (!notifications[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        // No empty slots, use the first one (oldest)
        slot = 0;
    }
    
    notification_t* notif = &notifications[slot];
    
    // Copy title and message
    strncpy(notif->title, title, sizeof(notif->title) - 1);
    notif->title[sizeof(notif->title) - 1] = '\0';
    
    strncpy(notif->message, message, sizeof(notif->message) - 1);
    notif->message[sizeof(notif->message) - 1] = '\0';
    
    notif->type = type;
    notif->timestamp = 0; // Will be set by the caller
    notif->active = 1;
    
    // Position notifications in top-right corner
    notif->x = fb_width - NOTIFICATION_WIDTH - 20;
    notif->y = 20 + (slot * (NOTIFICATION_HEIGHT + 10));
    
    if (slot >= notification_count) {
        notification_count = slot + 1;
    }
}

void notification_update(uint32_t current_time) {
    for (int i = 0; i < notification_count; i++) {
        if (notifications[i].active) {
            if (current_time - notifications[i].timestamp > NOTIFICATION_TIMEOUT) {
                notifications[i].active = 0;
            }
        }
    }
    
    // Compact the array
    int write_pos = 0;
    for (int read_pos = 0; read_pos < notification_count; read_pos++) {
        if (notifications[read_pos].active) {
            if (write_pos != read_pos) {
                notifications[write_pos] = notifications[read_pos];
                notifications[write_pos].y = 20 + (write_pos * (NOTIFICATION_HEIGHT + 10));
            }
            write_pos++;
        }
    }
    notification_count = write_pos;
}

void notification_draw(uint32_t* framebuffer, uint32_t width, uint32_t height) {
    (void)framebuffer; // Use global framebuffer
    (void)width;
    (void)height;
    
    for (int i = 0; i < notification_count; i++) {
        notification_t* notif = &notifications[i];
        if (!notif->active) continue;
        
        // Choose colors based on notification type
        uint32_t bg_color, border_color, text_color;
        switch (notif->type) {
            case NOTIFICATION_INFO:
                bg_color = 0x2196F3;
                border_color = 0x1976D2;
                text_color = 0xFFFFFF;
                break;
            case NOTIFICATION_WARNING:
                bg_color = 0xFF9800;
                border_color = 0xF57C00;
                text_color = 0xFFFFFF;
                break;
            case NOTIFICATION_ERROR:
                bg_color = 0xF44336;
                border_color = 0xD32F2F;
                text_color = 0xFFFFFF;
                break;
            case NOTIFICATION_SUCCESS:
                bg_color = 0x4CAF50;
                border_color = 0x388E3C;
                text_color = 0xFFFFFF;
                break;
            default:
                bg_color = current_theme.window_background;
                border_color = current_theme.window_border;
                text_color = current_theme.text_primary;
                break;
        }
        
        // Draw notification background
        fb_draw_rect(notif->x, notif->y, NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT, bg_color);
        
        // Draw border
        fb_draw_rect(notif->x, notif->y, NOTIFICATION_WIDTH, 2, border_color);
        fb_draw_rect(notif->x, notif->y + NOTIFICATION_HEIGHT - 2, NOTIFICATION_WIDTH, 2, border_color);
        fb_draw_rect(notif->x, notif->y, 2, NOTIFICATION_HEIGHT, border_color);
        fb_draw_rect(notif->x + NOTIFICATION_WIDTH - 2, notif->y, 2, NOTIFICATION_HEIGHT, border_color);
        
        // Draw title
        draw_string(notif->title, notif->x + 10, notif->y + 10, text_color);
        
        // Draw message
        draw_string(notif->message, notif->x + 10, notif->y + 30, text_color);
        
        // Draw close button (X)
        draw_string("X", notif->x + NOTIFICATION_WIDTH - 20, notif->y + 5, text_color);
    }
}

void notification_clear_all(void) {
    for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
        notifications[i].active = 0;
    }
    notification_count = 0;
}

// Convenience functions
void notify_info(const char* title, const char* message) {
    notification_show(title, message, NOTIFICATION_INFO);
}

void notify_warning(const char* title, const char* message) {
    notification_show(title, message, NOTIFICATION_WARNING);
}

void notify_error(const char* title, const char* message) {
    notification_show(title, message, NOTIFICATION_ERROR);
}

void notify_success(const char* title, const char* message) {
    notification_show(title, message, NOTIFICATION_SUCCESS);
}