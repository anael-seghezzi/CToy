/*======================================================================
 Maratis Tiny C Library
 version 1.0
------------------------------------------------------------------------
 Copyright (c) 2016 Anael Seghezzi <www.maratis3d.org>

 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would
    be appreciated but is not required.

 2. Altered source versions must be plainly marked as such, and must not
    be misrepresented as being the original software.

 3. This notice may not be removed or altered from any source
    distribution.

========================================================================*/
/*
   Floodfill-based path finding:
   
   to create the implementation,
   #define M_PF_IMPLEMENTATION
   in *one* C/CPP file that includes this file.
*/

#ifndef M_PF_H
#define M_PF_H

#define M_PF_VERSION 1

#ifndef MPFAPI
#ifdef __cplusplus
#define MPFAPI extern "C"
#else
#define MPFAPI extern
#endif
#endif

struct m_pf_point
{
   unsigned short x, y;
};

/* floodfill a score map from destination (x, y)
   the input map is filled with this two values: 0 = ground, UINT_MAX = wall
   *stack should be at least of size (w * h) */
MPFAPI void m_pf_floodfill(unsigned int *map, int w, int h, int x, int y, struct m_pf_point *stack);

/* backtrace a path from start (x, y) */
MPFAPI int m_pf_backtrace(struct m_pf_point *dest, const unsigned int *map, int w, int h, int x, int y);

#endif /* M_PF_H */

#ifdef M_PF_IMPLEMENTATION

#include <limits.h>

#define M_PF_PUSH_PIXEL(x2, y2)\
if(map[w * (y2) + (x2)] == 0) {\
   stack[stack_n].x = (x2);\
   stack[stack_n].y = (y2);\
   stack_n++;\
   map[w * (y2) + (x2)] = i;\
}

MPFAPI void m_pf_floodfill(unsigned int *map, int w, int h, int x, int y, struct m_pf_point *stack)
{
   unsigned int i = 0;
   int stack_c = 0;
   int stack_n = 0;

   M_PF_PUSH_PIXEL(x, y)

   while (stack_c < stack_n) {

      x = stack[stack_c].x;
      y = stack[stack_c].y;
      i = map[w * y + x] + 1;
      stack_c++;

      if (y > 0)
         M_PF_PUSH_PIXEL(x, y - 1)
      if (x > 0)
         M_PF_PUSH_PIXEL(x - 1, y)
      if (x < (w - 1))
         M_PF_PUSH_PIXEL(x + 1, y)
      if (y < (h - 1))
         M_PF_PUSH_PIXEL(x, y + 1)
   }
}

MPFAPI int m_pf_backtrace(struct m_pf_point *dest, const unsigned int *map, int w, int h, int x, int y)
{
   const unsigned int *p = map + (y * w + x);
   int i, s = w * h;

   if (*p == 0 || *p == UINT_MAX)
      return 0;

   for (i = 0; i < s; i++) {

      int minx, miny;
      int maxx, maxy;
      int xi, yi;
      int min = UINT_MAX;

      minx = (x - 1) < 0 ? 0 : (x - 1);
      miny = (y - 1) < 0 ? 0 : (y - 1);
      maxx = (x + 2) > w ? w : (x + 2);
      maxy = (y + 2) > h ? h : (y + 2);

      for (yi = miny; yi < maxy; yi++) {
         p = map + (yi * w + minx);
         for (xi = minx; xi < maxx; xi++) {

            if (*p < min) {
               min = *p;
               y = yi;
               x = xi;
            }
            p++;
         }
      }

      dest[i].x = x;
      dest[i].y = y;

      if (min == 1)
         return i + 1;
   }

   return 0;
}

#undef M_PF_PUSH_PIXEL

#endif /* M_PF_IMPLEMENTATION */