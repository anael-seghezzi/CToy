#ifndef _STDLIB_H
#define _STDLIB_H

#include "stddef.h"
#include "stdarg.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void *calloc(size_t nmemb, size_t size);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
int atoi(const char *nptr);
long int strtol(const char *nptr, char **endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);

void *bsearch(const void* key, const void* base, size_t num, size_t size, int (*compar)(const void*, const void*));
void qsort(void* base, size_t num, size_t size, int (*compar)(const void*, const void*));

#if defined(_WIN32)
int64_t _strtoi64(const char *str, char **endPtr, int radix);
int64_t _strtoui64(const char *str, char **endPtr, int radix);
#define strtoll _strtoi64
#define strtoull _strtoui64
#else
long long strtoll(const char *str, char **endPtr, int base);
long long strtoull(const char *str, char **endPtr, int base);
#endif

void exit(int);

#endif
