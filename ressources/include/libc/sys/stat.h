#ifndef _STAT_H
#define _STAT_H

#include "stddef.h"

struct stat {
   unsigned int   st_dev;
   unsigned short st_ino;
   unsigned short st_mode;
   short          st_nlink;
   short          st_uid;
   short          st_gid;
   unsigned int   st_rdev;
   long           st_size;
   int64_t        st_atime;
   int64_t        st_mtime;
   int64_t        st_ctime;
};

int ctoy__stat(const char *, struct stat *);
#define stat(f, s) ctoy__stat(f, s)

#if defined(_WIN32)
int _mkdir(const char *);
#define mkdir(f, o) _mkdir(f)
#else
int mkdir(const char *, unsigned int);
#endif

#endif
