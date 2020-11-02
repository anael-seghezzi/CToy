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
   Fast distance transform and Voronoi:
   
   to create the implementation,
   #define M_DIST_IMPLEMENTATION
   in *one* C/CPP file that includes this file.
   
   optional:
   include this file after *m_image.h* to enable m_image helpers
*/
   
#ifndef M_DIST_H
#define M_DIST_H

#define M_DIST_VERSION 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MDAPI
#define MDAPI extern
#endif

#define M_DIST_MAX 1e20f

/* raw distance functions
      dest: squared distance */
MDAPI void m_dist_transform_1d(float *dest, float *src, int count);
MDAPI void m_dist_transform_2d(float *dest, float *src, int width, int height);

/* raw voronoi distance functions
      destd: squared distance, desti: closest src index */
MDAPI void m_voronoi_transform_1d(float *destd, int *desti, float *src, int count);
MDAPI void m_voronoi_transform_2d(float *destd, int *desti, float *src, int width, int height);

/* image distance transform */
#ifdef M_IMAGE_VERSION

MDAPI void m_image_dist_mask_init(struct m_image *dest, const struct m_image *src); /* initialize a valid distance map from a ubyte mask */
MDAPI void m_image_dist_transform(struct m_image *dest, const struct m_image *src);
MDAPI void m_image_voronoi_transform(struct m_image *destd, struct m_image *desti, const struct m_image *src);
MDAPI void m_image_voronoi_fill(struct m_image *dest, const struct m_image *src, const struct m_image *srci);

#endif /* M_IMAGE_VERSION */

#ifdef __cplusplus
}
#endif
/*
----------------------------------------------------------------------*/
#endif /* M_DIST_H */