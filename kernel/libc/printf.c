#include <stdarg.h>
#include <stddef.h>
#include "stdio.h"
#include "string.h"

// Simple printf implementation for the kernel
int __printf_chk(int flag, const char *format, ...) {
    (void)flag; // Suppress unused parameter warning
    va_list args;
    va_start(args, format);
    
    int result = 0;
    char c;
    char *s;
    int i;
    
    while ((c = *format++) != '\0') {
        if (c != '%') {
            // Simple character output (replace with your actual output function)
            // For now, we'll just count the characters
            result++;
            continue;
        }
        
        // Handle format specifier
        c = *format++;
        switch (c) {
            case 's':
                s = va_arg(args, char *);
                while (*s) {
                    // Output character (replace with your actual output function)
                    result++;
                    s++;
                }
                break;
                
            case 'd':
            case 'i':
                i = va_arg(args, int);
                // Simple integer to string conversion (not handling negative numbers for simplicity)
                if (i == 0) {
                    result++;
                } else {
                    int temp = i;
                    while (temp > 0) {
                        temp /= 10;
                        result++;
                    }
                }
                break;
                
            case 'x':
            case 'X':
                i = va_arg(args, int);
                // Simple hex to string conversion
                if (i == 0) {
                    result++;
                } else {
                    int temp = i;
                    while (temp > 0) {
                        temp >>= 4;
                        result++;
                    }
                }
                break;
                
            case '%':
                // Output a literal '%'
                result++;
                break;
                
            default:
                // Unsupported format specifier, just output it as is
                result += 2; // for the '%' and the character
                break;
        }
    }
    
    va_end(args);
    return result;
}

// Forward declaration for text output
extern void text_putchar(char c);

static void print_char(char c) {
    text_putchar(c);
}

static void print_string(const char *s) {
    while (*s) {
        print_char(*s++);
    }
}

static void print_number(int num, int base) {
    if (num == 0) {
        print_char('0');
        return;
    }
    
    char buffer[32];
    int i = 0;
    int is_negative = 0;
    
    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }
    
    while (num > 0) {
        int digit = num % base;
        buffer[i++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        num /= base;
    }
    
    if (is_negative) {
        print_char('-');
    }
    
    while (i > 0) {
        print_char(buffer[--i]);
    }
}

// Standard printf implementation
int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    int result = 0;
    char c;
    
    while ((c = *format++) != '\0') {
        if (c != '%') {
            print_char(c);
            result++;
            continue;
        }
        
        c = *format++;
        switch (c) {
            case 's': {
                char *s = va_arg(args, char *);
                print_string(s);
                result += strlen(s);
                break;
            }
            case 'd':
            case 'i': {
                int i = va_arg(args, int);
                print_number(i, 10);
                result++;
                break;
            }
            case 'u': {
                unsigned int u = va_arg(args, unsigned int);
                print_number(u, 10);
                result++;
                break;
            }
            case 'x': {
                int i = va_arg(args, int);
                print_number(i, 16);
                result++;
                break;
            }
            case 'X': {
                int i = va_arg(args, int);
                print_number(i, 16);
                result++;
                break;
            }
            case 'c': {
                char ch = (char)va_arg(args, int);
                print_char(ch);
                result++;
                break;
            }
            case '%':
                print_char('%');
                result++;
                break;
            default:
                print_char('%');
                print_char(c);
                result += 2;
                break;
        }
    }
    
    va_end(args);
    return result;
}

// vsnprintf implementation that's compatible with the rest of the code
int vsnprintf(char *str, size_t size, const char *format, va_list args) {
    if (size == 0) return 0;
    
    size_t pos = 0;
    char c;
    
    while ((c = *format++) != '\0' && pos < size - 1) {
        if (c != '%') {
            str[pos++] = c;
            continue;
        }
        
        c = *format++;
        switch (c) {
            case 's': {
                char *s = va_arg(args, char *);
                while (*s && pos < size - 1) {
                    str[pos++] = *s++;
                }
                break;
            }
            case 'd':
            case 'i': {
                int num = va_arg(args, int);
                char buffer[32];
                int i = 0;
                int is_negative = 0;
                
                if (num == 0) {
                    if (pos < size - 1) str[pos++] = '0';
                    break;
                }
                
                if (num < 0) {
                    is_negative = 1;
                    num = -num;
                }
                
                while (num > 0) {
                    buffer[i++] = '0' + (num % 10);
                    num /= 10;
                }
                
                if (is_negative && pos < size - 1) {
                    str[pos++] = '-';
                }
                
                while (i > 0 && pos < size - 1) {
                    str[pos++] = buffer[--i];
                }
                break;
            }
            case 'u': {
                unsigned int num = va_arg(args, unsigned int);
                char buffer[32];
                int i = 0;
                
                if (num == 0) {
                    if (pos < size - 1) str[pos++] = '0';
                    break;
                }
                
                while (num > 0) {
                    buffer[i++] = '0' + (num % 10);
                    num /= 10;
                }
                
                while (i > 0 && pos < size - 1) {
                    str[pos++] = buffer[--i];
                }
                break;
            }
            case 'x':
            case 'X': {
                int num = va_arg(args, int);
                char buffer[32];
                int i = 0;
                
                if (num == 0) {
                    if (pos < size - 1) str[pos++] = '0';
                    break;
                }
                
                while (num > 0) {
                    int digit = num % 16;
                    buffer[i++] = (digit < 10) ? ('0' + digit) : 
                                 (c == 'x' ? 'a' + digit - 10 : 'A' + digit - 10);
                    num /= 16;
                }
                
                while (i > 0 && pos < size - 1) {
                    str[pos++] = buffer[--i];
                }
                break;
            }
            case 'c': {
                char ch = (char)va_arg(args, int);
                if (pos < size - 1) str[pos++] = ch;
                break;
            }
            case '%':
                if (pos < size - 1) str[pos++] = '%';
                break;
            default:
                if (pos < size - 2) {
                    str[pos++] = '%';
                    str[pos++] = c;
                }
                break;
        }
    }
    
    str[pos] = '\0';
    return pos;
}

// snprintf implementation using vsnprintf
int snprintf(char *str, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(str, size, format, args);
    va_end(args);
    return result;
}
