#ifndef _LOCALE_H
#define _LOCALE_H

#define LC_ALL          0
#define LC_COLLATE      1
#define LC_CTYPE        2
#define LC_MONETARY     3
#define LC_NUMERIC      4
#define LC_TIME         5
#define LC_MIN          LC_ALL
#define LC_MAX          LC_TIME

char *setlocale(int category, const char *locale);

#endif
