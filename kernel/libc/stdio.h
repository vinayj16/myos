#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>
#include <stddef.h>

// Function declarations
int printf(const char *format, ...);
int __printf_chk(int flag, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf(char *str, size_t size, const char *format, va_list args);
int __snprintf_chk(char *str, size_t size, int flag, size_t slen, const char *format, ...);

#endif // _STDIO_H