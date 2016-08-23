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

#ifdef M_RASTER_IMPLEMENTATION

#ifndef M_MIN
#define M_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef M_MAX
#define M_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef M_ABS
#define M_ABS(a) (((a) < 0) ? -(a) : (a))
#endif

#define _M_IS_IN_FRAME(x, y, w, h) (x >= 0 && x < w && y >= 0 && y < h)

#ifndef M_FIND_MIN_MAX_3
#define M_FIND_MIN_MAX_3(x0, x1, x2, min, max)\
   min = max = x0;\
   if (x1 < min) min=x1;\
   if (x1 > max) max=x1;\
   if (x2 < min) min=x2;\
   if (x2 > max) max=x2;
#endif

MRAPI void m_raster_inv_bilerp(float *dest, float x, float y, const float *v0, const float *v1, const float *v2, const float *v3)
{
   float vec1[2], vec2[2], vecp1[2], vecp2[2];
   float vA, vC, vB;
   float s, is, t, am2bpc, tdenom_x, tdenom_y;

   vec1[0] = v0[0] - v2[0];
   vec1[1] = v0[1] - v2[1];
   vec2[0] = v1[0] - v3[0];
   vec2[1] = v1[1] - v3[1];

   vecp1[0] = v0[0] - x;
   vecp1[1] = v0[1] - y;
   vecp2[0] = v1[0] - x;
   vecp2[1] = v1[1] - y;

   vA = vecp1[0] * vec1[1] - vecp1[1] * vec1[0];
   vC = vecp2[0] * vec2[1] - vecp2[1] * vec2[0];
   vB = ((vecp1[0] * vec2[1] - vecp1[1] * vec2[0]) +
         (vecp2[0] * vec1[1] - vecp2[1] * vec1[0])) * 0.5f;   
   
   am2bpc = vA - 2.0f * vB + vC;

   if (am2bpc > -0.0001f && am2bpc < 0.0001f)
      s = vA / (vA - vC);
   else
      s = ((vA - vB) + sqrtf(vB * vB - vA * vC)) / am2bpc;

   is = 1.0f - s;
   tdenom_x = is * vec1[0] + s * vec2[0];
   tdenom_y = is * vec1[1] + s * vec2[1];

   if (M_ABS(tdenom_x) > M_ABS(tdenom_y))
      t = (is * vecp1[0] + s * vecp2[0]) / tdenom_x;
   else
      t = (is * vecp1[1] + s * vecp2[1]) / tdenom_y;

   dest[0] = is;
   dest[1] = t;
}

MRAPI void m_raster_triangle_bbox_att4(float *dest, int width, int height, int minx, int miny, int maxx, int maxy, float *v0, float *v1, float *v2, float *a0, float *a1, float *a2)
{
   float *data = dest;
   float *yline;
   float up0[4], up1[4], up2[4];
   float denom, idenom;
   float t0_row, t1_row;
   float deltaX0, deltaX2, deltaY1, deltaY2;
   int w = width;
   int w4, mx4, y;

   /* denominator */
   denom = (v1[0] - v2[0]) * (v2[1] - v0[1]) - (v1[1] - v2[1]) * (v2[0] - v0[0]);
   if (denom >= 0.0f)
      return;
   
   idenom = 1.0f / denom;

   /* scanline */
   t0_row = ((v2[0] - v1[0]) * (miny-v1[1]) - (v2[1] - v1[1]) * (minx - v1[0])) * idenom;
   t1_row = ((v0[0] - v2[0]) * (miny-v2[1]) - (v0[1] - v2[1]) * (minx - v2[0])) * idenom;
   
   deltaX0 = (v0[0] - v2[0]) * idenom;
   deltaX2 = (v2[0] - v1[0]) * idenom;
   deltaY1 = (v1[1] - v2[1]) * idenom;
   deltaY2 = (v2[1] - v0[1]) * idenom;

   up0[0] = a0[0] * v0[3]; up0[1] = a0[1] * v0[3]; up0[2] = a0[2] * v0[3]; up0[3] = a0[3] * v0[3];
   up1[0] = a1[0] * v1[3]; up1[1] = a1[1] * v1[3]; up1[2] = a1[2] * v1[3]; up1[3] = a1[3] * v1[3];
   up2[0] = a2[0] * v2[3]; up2[1] = a2[1] * v2[3]; up2[2] = a2[2] * v2[3]; up2[3] = a2[3] * v2[3];

   w4 = w * 4;
   mx4 = minx * 4;
   
   yline = data + miny * w4;
   for (y = miny; y <= maxy; y++) {

      float *pixel = yline + mx4;
      float t0 = t0_row;
      float t1 = t1_row;
      int x;
      
      for (x = minx; x <= maxx; x++) {

         float t2 = t0 + t1;
         if (t0 >= 0.0f && t1 >= 0.0f && t2 <= 1.0f) {

            float it2 = 1.0f - t2;
            float w = 1.0f / (t0 * v0[3] + t1 * v1[3] + it2 * v2[3]);

            pixel[0] = (up0[0] * t0 + up1[0] * t1 + up2[0] * it2) * w;
            pixel[1] = (up0[1] * t0 + up1[1] * t1 + up2[1] * it2) * w;
            pixel[2] = (up0[2] * t0 + up1[2] * t1 + up2[2] * it2) * w;
            pixel[3] = (up0[3] * t0 + up1[3] * t1 + up2[3] * it2) * w;
         }

         t0 += deltaY1;
         t1 += deltaY2;
         pixel += 4;
      }

      t0_row += deltaX2;
      t1_row += deltaX0;
      yline += w4;
   }
}

MRAPI void m_raster_triangle_att4(float *dest, int width, int height, float *v0, float *v1, float *v2, float *a0, float *a1, float *a2)
{
   int minx, maxx, miny, maxy;
   int w = width;
   int h = height;

   if (v0[3] <= 0.0f || v1[3] <= 0.0f || v2[3] <= 0.0f) /* no clip */
      return;

   /* bounding box */
   M_FIND_MIN_MAX_3((int)v0[0], (int)v1[0], (int)v2[0], minx, maxx);
   M_FIND_MIN_MAX_3((int)v0[1], (int)v1[1], (int)v2[1], miny, maxy);
   minx = M_MAX(minx, 0);
   miny = M_MAX(miny, 0);
   maxx = M_MIN(maxx, w - 1);
   maxy = M_MIN(maxy, h - 1);

   m_raster_triangle_bbox_att4(dest, width, height, minx, miny, maxx, maxy, v0, v1, v2, a0, a1, a2);
}

MRAPI void m_raster_line(float *dest, int width, int height, int comp, float *p0, float *p1, float *color)
{
   float *data = dest;
   int x0 = (int)p0[0];
   int y0 = (int)p0[1];
   int x1 = (int)p1[0];
   int y1 = (int)p1[1];
   int w = width;
   int h = height;
   int dx =  M_ABS(x1 - x0), sx = x0 < x1 ? 1 : -1;
   int dy = -M_ABS(y1 - y0), sy = y0 < y1 ? 1 : -1;
   int err = dx + dy, e2;

   while (1) {

      if (_M_IS_IN_FRAME(x0, y0, w, h)) { /* safe, but should be taken out of the loop for speed (clipping ?) */
         float *pixel = data + (y0 * w + x0) * comp; int c;
         for (c = 0; c < comp; c++)
            pixel[c] = color[c];
      }
      
      if (x0 == x1 && y0 == y1)
         break;
      
      e2 = 2 * err;
      if (e2 >= dy) { err += dy; x0 += sx; }
      if (e2 <= dx) { err += dx; y0 += sy; }
   }
}

// Midpoint Circle Algorithm : http://en[3]ikipedia.org/wiki/Midpoint_circle_algorithm
MRAPI void m_raster_circle(float *dest, int width, int height, int comp, float *p, float r, float *color)
{
   #define _M_CIRCLE_PIXEL(px, py)\
   {\
      int _x = px;\
      int _y = py;\
      if (_M_IS_IN_FRAME(_x, _y, w, h)) {\
         float *pixel = data + (_y * w + _x) * comp; int c;\
         for (c = 0; c < comp; c++)\
            pixel[c] = color[c];\
      }\
   }

   float *data = dest;
   int x0 = (int)p[0];
   int y0 = (int)p[1];
   int radius = (int)r;
   int w = width;
   int h = height;
   int x = radius, y = 0;
   int radius_error = 1 - x;
 
   while (x >= y) {

      _M_CIRCLE_PIXEL( x + x0,  y + y0)
      _M_CIRCLE_PIXEL( y + x0,  x + y0)
      _M_CIRCLE_PIXEL(-x + x0,  y + y0)
      _M_CIRCLE_PIXEL(-y + x0,  x + y0)
      _M_CIRCLE_PIXEL(-x + x0, -y + y0)
      _M_CIRCLE_PIXEL(-y + x0, -x + y0)
      _M_CIRCLE_PIXEL( x + x0, -y + y0)
      _M_CIRCLE_PIXEL( y + x0, -x + y0)
      y++;
        
      if (radius_error < 0)
         radius_error += 2 * y + 1;
      else {
         x--;
         radius_error += 2 * (y - x + 1);
      }
   }

   #undef _M_CIRCLE_PIXEL
}

/* adapted from : http://alienryderflex.com/polygon_fill/
   public-domain code by Darel Rex Finley, 2007
*/
MRAPI void m_raster_polygon(float *dest, int width, int height, int comp, float *points, int count, float *color)
{
   float *data = dest;
   int *nodeX;
   int w = width;
   int h = height;
   int pixelY;
   int IMAGE_LEFT, IMAGE_RIGHT;
   int IMAGE_TOP, IMAGE_BOT;

   if (count < 3)
      return;
   
   nodeX = (int *)malloc(count * sizeof(int));
   
   /* bounding box */
   {
      float *point = points;
      float min[2], max[2]; int i;

      min[0] = point[0];
	   max[0] = point[0];
	   min[1] = point[1];
	   max[1] = point[1];
	   point += 2;

      for (i = 1; i < count; i++) {
         min[0] = M_MIN(min[0], point[0]);
         min[1] = M_MIN(min[1], point[1]);
         max[0] = M_MAX(max[0], point[0]);
         max[1] = M_MAX(max[1], point[1]);
         point += 2;
      }

      IMAGE_LEFT =  M_MAX(0, (int)min[0]);
      IMAGE_TOP =   M_MAX(0, (int)min[1]);
      IMAGE_RIGHT = M_MIN(w - 1, (int)max[0] + 1);
      IMAGE_BOT =   M_MIN(h - 1, (int)max[1] + 1);
   }

   /* loop through the rows of the image. */
   for (pixelY = IMAGE_TOP; pixelY < IMAGE_BOT; pixelY++) {

      /* build a list of nodes. */
      float *pointi, *pointj;
      int nodes = 0;
      int i, j;

      pointi = points;
      pointj = points + ((count - 1) * 2);

      for (i = 0; i < count; i++) {
         if ((pointi[1] < (float)pixelY && pointj[1] >= (float)pixelY) ||
             (pointj[1] < (float)pixelY && pointi[1] >= (float)pixelY))
            nodeX[nodes++] = (int)(pointi[0] + (pixelY - pointi[1]) / (pointj[1] - pointi[1]) * (pointj[0] - pointi[0]));
         pointj = pointi;
         pointi += 2;
      }

      /*  sort the nodes, via a simple “Bubble” sort. */
      i = 0;
      while (i < (nodes - 1)) {
         if (nodeX[i] > nodeX[i + 1]) {
            int swap = nodeX[i]; nodeX[i] = nodeX[i + 1]; nodeX[i + 1] = swap; if (i) i--;
         }
         else i++;
      }

      /* fill the pixels between node pairs. */
      for (i = 0; i < nodes; i += 2) {

         if (nodeX[i] >= IMAGE_RIGHT)
            break;
            
         if (nodeX[i + 1] > IMAGE_LEFT) {

            float *pixel;
            nodeX[i]     = M_MAX(nodeX[i], IMAGE_LEFT);
            nodeX[i + 1] = M_MIN(nodeX[i + 1], IMAGE_RIGHT);
            pixel = data + (pixelY * w + nodeX[i]) * comp;

            for (j = nodeX[i]; j < nodeX[i + 1]; j++) {
               int c;
               for (c = 0; c < comp; c++)
                  pixel[c] = color[c];
               pixel += comp;
            }
         }
      }
   }
   
   free(nodeX);
}

#endif /* M_RASTER_IMPLEMENTATION */