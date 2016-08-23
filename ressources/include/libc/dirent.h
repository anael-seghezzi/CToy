#ifndef _DIRENT_H
#define _DIRENT_H

#include "stddef.h"

#define DIR void

#if defined(_WIN32)
#define	DT_UNKNOWN  0
#define	DT_FIFO     0x1000
#define	DT_CHR      0x2000
#define	DT_DIR      0x4000 /* cross platform */
#define	DT_REG      0x8000 /* cross platform */

#else
#define	DT_UNKNOWN  0
#define	DT_FIFO     1
#define	DT_CHR      2
#define	DT_DIR      4
#define	DT_REG      8
#endif

#if defined(_WIN32)
struct dirent
{
   char d_name[261]; /* cross platform */
   size_t d_namlen;
   int d_type; /* partially cross platform (see DT_* above) */
};

#elif defined(__linux__)
struct dirent {
	uint64_t d_ino;
	int64_t d_off;
	uint16_t d_reclen;
	uint8_t	d_type;
	char d_name[256];
};

#else
struct dirent { /* OSX */
	uint32_t d_ino;
	uint16_t d_reclen;
	uint8_t  d_type;
	uint8_t d_namlen;
	char d_name[256];
};
#endif

DIR *opendir(const char *);
int closedir(DIR *);
struct dirent *readdir(DIR *);
void rewinddir(DIR *);

#endif
