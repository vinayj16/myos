#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

// String functions
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strncpy(char *dest, const char *src, size_t n);
char *__strncpy_chk(char *dest, const char *src, size_t n, size_t destlen);
char *strcpy(char *dest, const char *src);
size_t strlen(const char *s);

// Memory functions
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#endif // _STRING_H
