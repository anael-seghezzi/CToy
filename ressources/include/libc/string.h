#ifndef _STRING_H
#define _STRING_H

#include "stddef.h"
#include "stdarg.h"

char *strcat(char *dest, const char *src);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strcpy(char *dest, const char *src);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
char *strdup(const char *s);
char *strtok(char *s, const char *d);

#endif
