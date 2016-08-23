#ifndef _UNISTD_H
#define _UNISTD_H

#if defined(_WIN32)
int _rmdir(const char *);
#define rmdir _rmdir

#else
int rmdir(const char *);
#endif

#endif
