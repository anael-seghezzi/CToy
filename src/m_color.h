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
   Color conversion:
   - sRGB / linear
   - RGB, HSV, HSL
   
   to create the implementation,
   #define M_COLOR_IMPLEMENTATION
   in *one* C/CPP file that includes this file.
*/

#ifndef M_COLOR_H
#define M_COLOR_H

#define M_COLOR_VERSION 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MCAPI
#define MCAPI extern
#endif

/* Color conversion
   note: hue is in the range 0-360, all other values are in the range 0-1 */
MCAPI void m_color_sRGB_to_linear(const float *src, float *dest, int size);
MCAPI void m_color_linear_to_sRGB(const float *src, float *dest, int size);
MCAPI void m_color_RGB_to_HSV(const float *src, float *dest);
MCAPI void m_color_HSV_to_RGB(const float *src, float *dest);
MCAPI void m_color_RGB_to_HSL(const float *src, float *dest);
MCAPI void m_color_HSL_to_RGB(const float *src, float *dest);

#ifdef __cplusplus
}
#endif
/*
----------------------------------------------------------------------*/
#endif /* M_COLOR_H */

#ifdef M_COLOR_IMPLEMENTATION

#include <math.h>

#ifndef M_MIN
#define M_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef M_MAX
#define M_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

MCAPI void m_color_linear_to_sRGB(const float *src, float *dest, int size)
{
   int i;
   for (i = 0; i < size; i++) {
      if (*src < 0.0031308)
         *dest = 12.92 * (*src);
      else
         *dest = (1.0 + 0.055) * powf(*src, 1.0/2.4) - 0.055;
      dest++;
      src++;
   }
}

MCAPI void m_color_sRGB_to_linear(const float *src, float *dest, int size)
{
   int i;
   for (i = 0; i < size; i++) {
      if (*src <= 0.03928)
         *dest = *src / 12.92;
      else
         *dest = powf((*src + 0.055) / 1.055, 2.4);
      dest++;
      src++;
   }
}

MCAPI void m_color_RGB_to_HSV(const float *src, float *dest)
{
   float r = src[0];
   float g = src[1];
   float b = src[2];
   float h = 0;
   float s = 0;
   float v = 0;
   float min = r;
   float max = r;
   float delta;

   min = M_MIN(min, g);
   min = M_MIN(min, b);
   max = M_MAX(max, g);
   max = M_MAX(max, b);
   delta = max - min;
   v = max;

   if (delta == 0 || max == 0) {
      dest[0] = h; dest[1] = s; dest[2] = v;
      return;
   }

   s = delta / max;
   
   if (r == max)
      h = (g - b) / delta;
   else if (g == max)
      h = 2 + (b - r) / delta;
   else
      h = 4 + (r - g) / delta;
   
   h *= 60;
   if (h < 0) h += 360;

   dest[0] = h; dest[1] = s; dest[2] = v;
}

MCAPI void m_color_HSV_to_RGB(const float *src, float *dest)
{
   float r, g, b;
   float f, p, q, t;
   float h = src[0];
   float s = src[1];
   float v = src[2];
   int i;
   
   if (s == 0) {
      dest[0] = v; dest[1] = v; dest[2] = v;
      return;
   }
   
   h /= 60.0f;
   i = (int)floorf(h);
   f = h - i;
   p = v * (1 - s);
   q = v * (1 - s * f);
   t = v * (1 - s * (1 - f));
   
   switch (i) {
   case 0:
      r = v; g = t; b = p;
      break;
   case 1:
      r = q; g = v; b = p;
      break;
   case 2:
      r = p; g = v; b = t;
      break;
   case 3:
      r = p; g = q; b = v;
      break;
   case 4:
      r = t; g = p; b = v;
      break;
   default:
      r = v; g = p; b = q;
      break;
   }
   
   dest[0] = r; dest[1] = g; dest[2] = b;
}

MCAPI void m_color_RGB_to_HSL(const float *src, float *dest)
{
   float h, s, l, dr, dg, db;
   float r = src[0];
   float g = src[1];
   float b = src[2];
   float min = r;
   float max = r;
   float delta;

   min = M_MIN(min, g);
   min = M_MIN(min, b);
   max = M_MAX(max, g);
   max = M_MAX(max, b);

   delta = max - min;
   h = 0;
   s = 0;
   l = (max + min) * 0.5f;
   
   if (max == 0) {
      dest[0] = h; dest[1] = s; dest[2] = l;
      return;
   }

   if(r == max)
      h = fmodf(((g - b) / delta), 6.0f);
   else if(g == max)
      h = ((b - r) / delta) + 2.0f;
   else
      h = ((r - g) / delta) + 4.0f;

   h *= 60.0f;
   if (h < 0) h += 360;
    
   s = delta / (1.0f - fabsf(2.0f * l - 1.0f));
   
   dest[0] = h;
   dest[1] = s;
   dest[2] = l;
}

MCAPI void m_color_HSL_to_RGB(const float *src, float *dest)
{
   float h = src[0];
   float s = src[1];
   float l = src[2];
   float c, m, x;
   
   if (s == 0) {
      dest[0] = l; dest[1] = l; dest[2] = l;
      return;
   }
   
   c = (1.0f - fabsf(2.0f * l - 1.0f)) * s;
   m = 1.0f * (l - 0.5f * c);
   x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2) - 1.0f));

   if (h >= 0.0f && h < 60.0f) {
      dest[0] = c + m;
      dest[1] = x + m;
      dest[2] = m;
   }
   else if (h >= 60.0f && h < 120.0f) {
      dest[0] = x + m;
      dest[1] = c + m;
      dest[2] = m;
   }
   else if (h < 120.0f && h < 180.0f) {
      dest[0] = m;
      dest[1] = c + m;
      dest[2] = x + m;
   }
   else if (h >= 180.0f && h < 240.0f) {
      dest[0] = m;
      dest[1] = x + m;
      dest[2] = c + m;
   }
   else if (h >= 240.0f && h < 300.0f) {
      dest[0] = x + m;
      dest[1] = m;
      dest[2] = c + m;
   }
   else if (h >= 300.0f && h < 360.0f) {
      dest[0] = c + m;
      dest[1] = m;
      dest[2] = x + m;
   }
   else {
     dest[0] = m;
     dest[1] = m;
     dest[2] = m;
   }
}

#endif /* M_COLOR_IMPLEMENTATION */