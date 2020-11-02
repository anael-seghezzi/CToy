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