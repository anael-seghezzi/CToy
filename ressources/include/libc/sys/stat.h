#ifndef _STAT_H
#define _STAT_H

#include "stddef.h"

#if defined(_WIN32)
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

#elif defined(__linux__)
struct stat {
	dev_t	st_dev;
	ino_t	st_ino;
	mode_t	st_mode;
	nlink_t	st_nlink;
	uid_t	st_uid;
	gid_t	st_gid;
	dev_t	st_rdev;
	off_t	st_size;
	time_t	st_atime;
	long	st_spare1; /* not cross platform */
	time_t	st_mtime;
	long	st_spare2; /* not cross platform */
	time_t	st_ctime;
/* following not cross platform */
	long	st_spare3;
	long	st_blksize;
	long	st_blocks;
	u_long	st_flags;
	u_long	st_gen;
};

#else
struct stat { /* OSX */
	int32_t  st_dev;
	uint32_t st_ino;
	uint16_t st_mode;
	uint16_t st_nlink;
	uint32_t st_uid;
	uint32_t st_gid;
	int32_t  st_rdev;
	int64_t  st_atime;
	int64_t  st_atimensec; /* not cross platform */
	int64_t  st_mtime;
	int64_t  st_mtimensec; /* not cross platform */
	int64_t  st_ctime;
	int64_t  st_ctimensec; /* not cross platform */
	int64_t  st_size;
/* following not cross platform */
	int64_t  st_blocks;
	int32_t  st_blksize;
	uint32_t st_flags;
	uint32_t st_gen;
	int32_t  st_lspare;
	int64_t  st_qspare[2];
};
#endif

#if defined(_WIN32)
int _stat64i32(const char *, struct stat *);
int _mkdir(const char *);
#define stat _stat64i32
#define mkdir(f, o) _mkdir(f)

#else
int stat(const char *, struct stat *);
int mkdir(const char *, unsigned int);
#endif

#endif
