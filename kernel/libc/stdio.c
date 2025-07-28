#include "stdio.h"
#include "string.h"
#include <stdarg.h>

// Fortified version of snprintf with buffer overflow checking
int __snprintf_chk(char *str, size_t size, int flag, size_t slen, const char *format, ...) {
    (void)flag;  // Unused parameter
    if (size > slen) {
        // Buffer overflow would occur
        return -1;
    }
    
    va_list args;
    va_start(args, format);
    int result = vsnprintf(str, size, format, args);
    va_end(args);
    
    return result;
}
