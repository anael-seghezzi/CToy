#ifndef __ASSERT_H_
#define __ASSERT_H_

#ifdef NDEBUG
#ifndef assert
#define assert(ignore)((void) 0)
#endif
#else

#ifndef assert
#define __assert(e, file, line) \
    ((void)printf ("%s:%u: failed assertion `%s'\n", file, line, e))
#define assert(e) \
    ((void) ((e) ? 0 : __assert (#e, __FILE__, __LINE__)))
#endif
#endif

#endif
