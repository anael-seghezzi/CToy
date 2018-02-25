/*======================================================================
 Maratis Tiny C Library
 version 1.0
------------------------------------------------------------------------
 Copyright (c) 2015 Anael Seghezzi <www.maratis3d.org>
 Copyright (c) 2015 Marti Maria Saguer

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
   Image manipulation :
   - transformation (re-frame, mirror, rotation)
   - conversions (float, half, ubyte, linear, greyscale...)
   - filtering (convolution, Gaussian blur, Harris)
   - scaling (pyramid, generic, bilinear)
   - morphology (flood-fill, dilate, erode, thinning)
   - edge and corner detection (Sobel, Harris)

   to create the implementation,
   #define M_IMAGE_IMPLEMENTATION
   in *one* C/CPP file that includes this file.

   optional:
   include after *m_math.h*
   
   //////////////////////////////////////////////////////
   Example: create a 256x256 float image with 1 component:
 
   struct m_image foo1 = M_IMAGE_IDENTITY();
   struct m_image foo2 = M_IMAGE_IDENTITY();   
   int x, y;
   
   m_image_create(&foo1, M_FLOAT, 256, 256, 1);
   memset(foo1.data, 0, foo1.size * sizeof(float)); // clear to zero
   
   y = 128; x = 128;
   ((float *)foo1.data)[y * foo1.width + x] = 1.0f; // set (x, y) pixel to one
   
   m_image_gaussian_blur(&foo2, &foo1, 3, 3); // apply Gaussian blur
   
   m_image_destroy(&foo2);
   m_image_destroy(&foo1);
*/

#ifndef M_IMAGE_H
#define M_IMAGE_H

#include <stdint.h>

#define M_IMAGE_VERSION 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MIAPI
#define MIAPI extern
#endif

#define M_VOID   0
#define M_BOOL   1
#define M_BYTE   2
#define M_UBYTE  3
#define M_SHORT  4
#define M_USHORT 5
#define M_INT    6
#define M_UINT   7
#define M_HALF   8
#define M_FLOAT  9
#define M_DOUBLE 10

struct m_image
{
   void *data;
   int size;
   int width;
   int height;
   int comp;
   char type;
};

/* identity, must be used before calling m_image_create */
#define M_IMAGE_IDENTITY() {0, 0, 0, 0, 0, 0}

/* m_image type util */
MIAPI int m_type_sizeof(char type);

/* fully supported types are: M_UBYTE, M_USHORT, M_HALF, M_FLOAT
   partially supported types: M_BYTE, M_SHORT, M_INT, M_UINT (no support for conversion) */
MIAPI void m_image_create(struct m_image *image, char type, int width, int height, int comp);
MIAPI void m_image_destroy(struct m_image *image);

MIAPI void m_image_ubyte_to_float(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_ushort_to_float(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_half_to_float(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_float_to_ubyte(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_float_to_ushort(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_float_to_half(struct m_image *dest, const struct m_image *src);

MIAPI void m_image_copy(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_copy_sub_image(struct m_image *dest, const struct m_image *src, int x, int y, int w, int h);
MIAPI void m_image_reframe_zero(struct m_image *dest, const struct m_image *src, int left, int top, int right, int bottom);
MIAPI void m_image_reframe(struct m_image *dest, const struct m_image *src, int left, int top, int right, int bottom);
MIAPI void m_image_extract_component(struct m_image *dest, const struct m_image *src, int c);
MIAPI void m_image_rotate_left(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_rotate_right(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_rotate_180(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_mirror_x(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_mirror_y(struct m_image *dest, const struct m_image *src);

MIAPI void m_image_premultiply(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_unpremultiply(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_sRGB_to_linear(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_linear_to_sRGB(struct m_image *dest, const struct m_image *src);

/* float/half conversion */
MIAPI float    m_half2float(uint16_t h);
MIAPI uint16_t m_float2half(float flt);

/* raw processing */
MIAPI void  m_sRGB_to_linear(float *dest, const float *src, int size);
MIAPI void  m_linear_to_sRGB(float *dest, const float *src, int size);
MIAPI void  m_RGB_to_HSV(float *dest, const float *src);
MIAPI void  m_HSV_to_RGB(float *dest, const float *src);
MIAPI void  m_RGB_to_HSL(float *dest, const float *src);
MIAPI void  m_HSL_to_RGB(float *dest, const float *src);
MIAPI void  m_gaussian_kernel(float *dest, int size, float radius);
MIAPI void  m_sst(float *dest, const float *src, int count);
MIAPI void  m_harris_response(float *dest, const float *src, int count);
MIAPI void  m_tfm(float *dest, const float *src, int count);
MIAPI void  m_normalize(float *dest, const float *src, int size); /* dest = src / norm(src) */
MIAPI void  m_normalize_sum(float *dest, const float *src, int size); /* dest = src / sum(src) */
MIAPI float m_mean(const float *src, int size);
MIAPI float m_squared_distance(const float *src1, const float *src2, int size);
MIAPI float m_convolution(const float *src1, const float *src2, int size); /* a dot product really */
MIAPI float m_chi_squared_distance(const float *src1, const float *src2, int size); /* good at estimating signed hystograms difference */

/* conversion to 1 component (float image only) */
MIAPI void m_image_grey(struct m_image *dest, const struct m_image *src); /* from RGB src */
MIAPI void m_image_max(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_max_abs(struct m_image *dest, const struct m_image *src);

/* summed area table (also called "integral image") */
MIAPI void m_image_summed_area(struct m_image *dest, const struct m_image *src);

/* convolutions (float image only) */
/* if alpha channel, src image must be pre-multiplied */
MIAPI void m_image_convolution_h_raw(struct m_image *dest, const struct m_image *src, float *kernel, int size);
MIAPI void m_image_convolution_v_raw(struct m_image *dest, const struct m_image *src, float *kernel, int size);
MIAPI void m_image_convolution_h(struct m_image *dest, const struct m_image *src, float *kernel, int size); /* horizontal */
MIAPI void m_image_convolution_v(struct m_image *dest, const struct m_image *src, float *kernel, int size); /* vertical */
MIAPI void m_image_gaussian_blur(struct m_image *dest, const struct m_image *src, float dx, float dy);

/* edge and corner (float 1 component image only) */
MIAPI void m_image_sobel(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_harris(struct m_image *dest, const struct m_image *src, float radius);

/* morphology (ubyte 1 component image only) */
MIAPI int  m_image_floodfill_4x(struct m_image *dest, int x, int y, uint8_t ref, uint8_t value, uint16_t *stack, int stack_size);
MIAPI int  m_image_floodfill_8x(struct m_image *dest, int x, int y, uint8_t ref, uint8_t value, uint16_t *stack, int stack_size);
MIAPI void m_image_dilate(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_erode(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_edge_4x(struct m_image *dest, const struct m_image *src, uint8_t ref);
MIAPI void m_image_thin(struct m_image *dest);

/* non maxima suppression (float image only) */
MIAPI void m_image_non_max_supp(struct m_image *dest, const struct m_image *src, int radius, float threshold);

/* detect Harris corners
   margin: margin around the image to exclude corners
   radius: maxima radius
   threshold: Harris response threshold
   corners: corners coordinates of size max_count * 2
   max_count: maximum number of corners
   return corner count */
MIAPI int m_image_corner_harris(const struct m_image *src, int margin, float radius, float threshold, int *corners, int max_count);

/* resizing (float image only) */
MIAPI void m_image_sub_pixel(const struct m_image *src, float x, float y, float *result);
MIAPI void m_image_pyrdown(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_resize(struct m_image *dest, const struct m_image *src, int new_width, int new_height);

#ifdef __cplusplus
}
#endif
/*
----------------------------------------------------------------------*/
#endif /* M_IMAGE_H */
