/*======================================================================
 Maratis Tiny C Library
 version 1.0
------------------------------------------------------------------------
 Copyright (c) 2015 Anael Seghezzi <www.maratis3d.org>

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
   Simple rasterization:
   - triangle with perspective correct interpolation
   - basic line, circle and polygon
   
   to create the implementation,
   #define M_RASTER_IMPLEMENTATION
   in *one* C/CPP file that includes this file.
*/

#ifndef M_RASTER_H
#define M_RASTER_H

#define M_RASTER_VERSION 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MRAPI
#define MRAPI extern
#endif

/* inverse bilinear interpolation */
MRAPI void m_raster_inv_bilerp(float *dest, float x, float y, const float *v0, const float *v1, const float *v2, const float *v3);

/* triangle with 4 components attributes (float 4 vertices and attributes) */
MRAPI void m_raster_triangle_bbox_att4(float *dest, int width, int height, int minx, int miny, int maxx, int maxy, float *v0, float *v1, float *v2, float *a0, float *a1, float *a2);
MRAPI void m_raster_triangle_att4(float *dest, int width, int height, float *v0, float *v1, float *v2, float *a0, float *a1, float *a2);

/* basic 2d rasterization (float 2 points) */
MRAPI void m_raster_line(float *dest, int width, int height, int comp, float *p0, float *p1, float *color);
MRAPI void m_raster_circle(float *dest, int width, int height, int comp, float *p, float r, float *color);
MRAPI void m_raster_polygon(float *dest, int width, int height, int comp, float *points, int count, float *color);

#ifdef __cplusplus
}
#endif
/*
----------------------------------------------------------------------*/
#endif /* M_RASTER_H */