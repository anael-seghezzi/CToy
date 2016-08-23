#ifndef _MEMORY_H
#define _MEMORY_H

#include "stddef.h"
#include "stdarg.h"

int memcmp(const void *_Buf1,const void *_Buf2,size_t _Size);
void *memcpy(void *_Dst,const void *_Src,size_t _Size);
void *memset(void *_Dst,int _Val,size_t _Size);

#endif
