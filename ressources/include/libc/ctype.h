#ifndef _CTYPE_H
#define _CTYPE_H

#include "stddef.h"
#include "stdarg.h"

#define _UPPER 0x1
#define _LOWER 0x2
#define _DIGIT 0x4
#define _SPACE 0x8

#define _PUNCT 0x10
#define _CONTROL 0x20
#define _BLANK 0x40
#define _HEX 0x80

#define _LEADBYTE 0x8000
#define _ALPHA (0x0100|_UPPER|_LOWER)

int isalpha(int _C);
int isupper(int _C);
int islower(int _C);
int isdigit(int _C);
int isxdigit(int _C);
int isspace(int _C);
int ispunct(int _C);
int isalnum(int _C);
int isprint(int _C);
int isgraph(int _C);
int iscntrl(int _C);
int toupper(int _C);
int tolower(int _C);

#endif
