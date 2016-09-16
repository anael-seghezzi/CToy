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

#ifdef M_DIST_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <assert.h>

/* From paper:
   Distance Transforms of Sampled Functions
   by Pedro F. Felzenszwalb and Daniel P. Huttenlocher */

#define M_DIST_T()\
v[0] = 0;\
z[0] = -M_DIST_MAX;\
z[1] = M_DIST_MAX;\
k = 0;\
for (q = 1; q < count; q++) {\
   float s;\
   while (1) {\
      s = ((src[q] + q * q) - (src[v[k]] + v[k] * v[k])) / (float)(2 * q - 2 * v[k]);\
      if (s > z[k])\
         break;\
      k--;\
   }\
   k++;\
   v[k] = q;\
   z[k] = s;\
   z[k+1] = M_DIST_MAX;\
}

void m_dist_transform_1d(float *dest, float *src, int count)
{
   int *v = (int *)malloc(count * sizeof(int));
   float *z = (float *)malloc((count + 1) * sizeof(float));
   int q, k;

   M_DIST_T()

   k = 0;
   for (q = 0; q < count; q++) {
      while(z[k+1] < q) k++;
      dest[q] = (q - v[k]) * (q - v[k]) + src[v[k]];
   }
   
   free(z);
   free(v);
}

void m_voronoi_transform_1d(float *destd, int *desti, float *src, int count)
{
   int *v = (int *)malloc(count * sizeof(int));
   float *z = (float *)malloc((count + 1) * sizeof(float));
   int q, k;

   M_DIST_T()
   
   k = 0;
   for (q = 0; q < count; q++) {
      while (z[k+1] < q) k++;
      destd[q] = (q - v[k]) * (q - v[k]) + src[v[k]];
      desti[q] = v[k];
   }

   free(z);
   free(v);
}

void m_dist_transform_2d(float *dest, float *src, int width, int height)
{
   float *tmp1 = (float *)malloc(M_MAX(width, height) * sizeof(float));
   float *tmp2 = (float *)malloc(M_MAX(width, height) * sizeof(float));
   int x, y;

   /* vertical pass */
   for (x = 0; x < width; x++) {

      /* fill vertical line */
      for (y = 0; y < height; y++)
         tmp1[y] = *(src + width * y + x);

      m_dist_transform_1d(tmp2, tmp1, height);

      /* copy the result */
      for (y = 0; y < height; y++)
         *(dest + width * y + x) = tmp2[y];
   }

   /* horizontal pass */
   for (y = 0; y < height; y++) {
      memcpy(tmp1, dest + width * y, width * sizeof(float));
      m_dist_transform_1d(dest + width * y, tmp1, width);
   }

   free(tmp2);
   free(tmp1);
}

void m_voronoi_transform_2d(float *destd, int *desti, float *src, int width, int height)
{
   int maxs = M_MAX(width, height);
   float *tmp1 = (float *)malloc(maxs * sizeof(float));
   float *tmp2 = (float *)malloc(maxs * sizeof(float));
   int *tmpi1 = (int *)malloc(maxs * sizeof(int));
   int *tmpi2 = (int *)malloc(width * sizeof(int));
   int x, y;

   /* vertical pass */
   for (x = 0; x < width; x++) {

      /* fill vertical line */
      for (y = 0; y < height; y++)
         tmp1[y] = *(src + width * y + x);

      m_voronoi_transform_1d(tmp2, tmpi1, tmp1, height);

      /* copy the result */
      for (y = 0; y < height; y++) {
         *(destd + width * y + x) = tmp2[y];
         *(desti + width * y + x) = tmpi1[y];
      }
   }

   /* horizontal pass */
   for (y = 0; y < height; y++) {

      /* copy distance and index of current line */
      memcpy(tmp1, destd + width * y, width * sizeof(float));
      memcpy(tmpi2, desti + width * y, width * sizeof(int));
      
      m_voronoi_transform_1d(destd + width * y, tmpi1, tmp1, width);

      /* compute 2d closest pixel index */
      for (x = 0; x < width; x++) {
         int _x = tmpi1[x];
         int _y = tmpi2[_x];
         *(desti + width * y + x) = _y * width + _x;
      }
   }

   free(tmpi2);
   free(tmpi1);
   free(tmp2);
   free(tmp1);
}

#ifdef M_IMAGE_VERSION

void m_image_dist_mask_init(struct m_image *dest, const struct m_image *src)
{
   float *dest_data;
   unsigned char *src_data;
   int width = src->width;
   int height = src->height;
   int size = src->size;
   int i;

   assert(src->size > 0 && src->type == M_UBYTE && src->comp == 1);
   m_image_create(dest, M_FLOAT, width, height, 1);

   dest_data = (float *)dest->data;
   src_data = (unsigned char *)src->data;

   /* init from mask */
   for (i = 0; i < size; i++)
      dest_data[i] = src_data[i] < 128 ? M_DIST_MAX : 0;
}

void m_image_dist_transform(struct m_image *dest, const struct m_image *src)
{
   float *dest_data;
   int width = src->width;
   int height = src->height;
   int size = src->size;
   int i;
   
   assert(src->size > 0 && src->type == M_FLOAT && src->comp == 1);
   m_image_create(dest, M_FLOAT, width, height, 1);
   dest_data = (float *)dest->data;

   /* distance transform */
   m_dist_transform_2d(dest_data, (float *)src->data, width, height);

   for (i = 0; i < size; i++)
      dest_data[i] = sqrtf(dest_data[i]);
}

void m_image_voronoi_transform(struct m_image *destd, struct m_image *desti, const struct m_image *src)
{
   float *dest_data;
   int width = src->width;
   int height = src->height;
   int size = src->size;
   int i;
   
   assert(src->size > 0 && src->type == M_FLOAT && src->comp == 1);
   m_image_create(destd, M_FLOAT, width, height, 1);
   m_image_create(desti, M_INT, width, height, 1);
   dest_data = (float *)destd->data;

   /* distance transform */
   m_voronoi_transform_2d(dest_data, (int *)desti->data, (float *)src->data, width, height);

   for (i = 0; i < size; i++)
      dest_data[i] = sqrtf(dest_data[i]);
}

void m_image_voronoi_fill(struct m_image *dest, const struct m_image *src, const struct m_image *srci)
{
   #define M_VORO_FILL(T)\
   {\
      T *sData = (T *)src->data;\
      T *dData = (T *)dest->data;\
      int i, c;\
      for (i = 0; i < size; i++) {\
         for (c = 0; c < comp; c++) {\
            (*dData) = sData[(*datai) * comp + c];\
            dData++;\
         }\
         datai++;\
      }\
   }

   int *datai = (int *)srci->data;
   int comp = src->comp;
   int size = srci->size;

   m_image_create(dest, src->type, src->width, src->height, src->comp);

   switch(src->type)
   {
   case M_BYTE:
   case M_UBYTE:
      M_VORO_FILL(char);
      break;
   case M_SHORT:
   case M_USHORT:
   case M_HALF:
      M_VORO_FILL(short);
      break;
   case M_INT:
   case M_UINT:
      M_VORO_FILL(int);
      break;
   case M_FLOAT:
      M_VORO_FILL(float);
      break;
   default:
      assert(0);
      break;
   }
   
   #undef M_VORO_FILL
}

#endif /* M_IMAGE_VERSION */

#endif /* M_DIST_IMPLEMENTATION */