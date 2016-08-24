#ifndef _STDIO_H
#define _STDIO_H

#include "stddef.h"
#include "stdarg.h"

typedef struct __FILE FILE;
typedef int64_t fpos_t;

#define EOF (-1)
#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 0
#define FILENAME_MAX 256

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

int remove(const char *);
int rename(const char *oldname, const char *newname);

int fclose(FILE *stream);
int fflush(FILE *stream);
FILE *fopen(const char *path, const char *mode);
FILE *freopen(const  char *path, const char *mode, FILE *stream);
void setbuf(FILE *stream, char *buffer);
int setvbuf(FILE *stream, char *buffer, int mode, size_t size);

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(void *ptr, size_t size, size_t nmemb, FILE *stream);

int fprintf(FILE *stream, const char *format, ...);
int fscanf(FILE *_File, const char *format, ...);
int printf(const char *format, ...);
int scanf(const char *format, ...);
int snprintf(char *str, size_t size, const  char  *format, ...);
int sprintf(char *str, const char *format, ...);
int sscanf(const char *src,const char *format, ...);
int vfprintf(FILE  *stream,  const  char *format, va_list ap);
int vfscanf(FILE * stream, const char *format, va_list arg);
int vprintf(const char *format, va_list ap);
int vscanf(const char *format, va_list arg);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int vsprintf(char *str, const char *format, va_list ap);
int vsscanf(const char *s, const char *format, va_list arg);

int fgetc(FILE *stream);
char *fgets(char *s, int size, FILE *stream);
int fputc(int character, FILE *stream);
int fputs(const char *str, FILE *stream);
int getc(FILE *stream);
int getchar(void);
char *gets(char *s);
int putc(int character, FILE *stream);
int putchar(int character);
int puts(const char * str);
int ungetc(int c, FILE *stream);

int fgetpos(FILE *stream, fpos_t *pos);
int fseek(FILE *stream, long int offset, int origin);
int fsetpos(FILE *stream, const fpos_t *pos);
long int ftell(FILE *stream);
int rewind(FILE *stream);

void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE * stream);
void perror(const char *s);

#endif
