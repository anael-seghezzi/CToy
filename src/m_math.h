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
   Math with C/OpenCL portability:
   - vector manipulation
   - interpolation (cubic, catmullrom)
   - quaternion basics
   - matrix (projection, transformation...)
   - random number generator
   - 2d routines
   - 3d routines:
      - voxeliser (tri-box overlap)
      - raytracing (sphere, plane, box, triangle)
   
   to create the implementation,
   #define M_MATH_IMPLEMENTATION
   in *one* C/CPP file that includes this file.
*/

#ifndef M_MATH_H
#define M_MATH_H

#define M_MATH_VERSION 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MMAPI
#define MMAPI extern
#endif

/* basic math */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define M_DEG_TO_RAD 0.01745329251994329576
#define M_RAD_TO_DEG 57.29577951308232087679

#ifndef M_MIN
#define M_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef M_MAX
#define M_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef M_ABS
#define M_ABS(a) (((a) < 0) ? -(a) : (a))
#endif
#ifndef M_CLAMP
#define M_CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

#define M_ISPOWEROFTWO(x) (((x&(x - 1)) == 0) && (x != 0))

/* vector math */
typedef struct {float x, y, z;} float3; /* float3 (doesn't exist in opencl) */

/* opencl/c */
#ifdef __OPENCL_VERSION__
   #define M_DOT2(A, B) dot((A).xy, (B).xy)
   #define M_DOT3(A, B) dot((float4)((A).x, (A).y, (A).z, 0), (float4)((B).x, (B).y, (B).z, 0.0f))
   #define M_DOT4(A, B) dot(A, B)
   #define M_LENGHT2(src) length(src)
   #define M_LENGHT3(src) length((float4)((src).x, (src).y, (src).z, 0.0f))
   #define M_LENGHT4(src) length(src)
   #define M_MIN2(dest, A, B) (dest).xy = min(A.xy, B.xy);
   #define M_MIN3(dest, A, B) (dest).x = M_MIN((A).x, (B).x); (dest).y = M_MIN((A).y, (B).y); (dest).z = M_MIN((A).z, (B).z);
   #define M_MIN4(dest, A, B) (dest) = min(A, B);
   #define M_MAX2(dest, A, B) (dest).xy = max(A.xy, B.xy);
   #define M_MAX3(dest, A, B) (dest).x = M_MAX((A).x, (B).x); (dest).y = M_MAX((A).y, (B).y); (dest).z = M_MAX((A).z, (B).z);
   #define M_MAX4(dest, A, B) (dest) = max(A, B);
   #define M_NORMALIZE2(dest, src) (dest).xy = normalize((src).xy);
   #define M_NORMALIZE4(dest, src) (dest) = normalize(src);
#else
   typedef struct {float x, y;} float2;       /* float2 */
   typedef struct {float x, y, z, w;} float4; /* float4 */
   #define M_DOT2(A, B) ((A).x * (B).x + (A).y * (B).y)
   #define M_DOT3(A, B) ((A).x * (B).x + (A).y * (B).y + (A).z * (B).z)
   #define M_DOT4(A, B) ((A).x * (B).x + (A).y * (B).y + (A).z * (B).z + (A).w * (B).w)
   #define M_LENGHT2(src) sqrtf((src).x * (src).x + (src).y * (src).y)
   #define M_LENGHT3(src) sqrtf((src).x * (src).x + (src).y * (src).y + (src).z * (src).z)
   #define M_LENGHT4(src) sqrtf((src).x * (src).x + (src).y * (src).y + (src).z * (src).z + (src).w * (src).w)
   #define M_MIN2(dest, A, B) (dest).x = M_MIN((A).x, (B).x); (dest).y = M_MIN((A).y, (B).y);
   #define M_MIN3(dest, A, B) M_MIN2(dest, A, B); (dest).z = M_MIN((A).z, (B).z);
   #define M_MIN4(dest, A, B) M_MIN3(dest, A, B); (dest).w = M_MIN((A).w, (B).w);
   #define M_MAX2(dest, A, B) (dest).x = M_MAX((A).x, (B).x); (dest).y = M_MAX((A).y, (B).y);
   #define M_MAX3(dest, A, B) M_MAX2(dest, A, B); (dest).z = M_MAX((A).z, (B).z);
   #define M_MAX4(dest, A, B) M_MAX3(dest, A, B); (dest).w = M_MAX((A).w, (B).w);
   #define M_NORMALIZE2(dest, src){ float l = M_LENGHT2(src);\
      if (l > 0) { l = 1.0f / l; (dest).x = (src).x * l; (dest).y = (src).y * l; }\
      else { (dest).x = (dest).y = 0.0f; }}
   #define M_NORMALIZE4(dest, src){ float l = M_LENGHT4(src);\
      if (l > 0) { l = 1.0f / l; (dest).x = (src).x * l; (dest).y = (src).y * l; (dest).z = (src).z * l; (dest).w = (src).w * l; }\
      else { (dest).x = (dest).y = (dest).z = (dest).w = 0.0f; }}
#endif

#define M_NORMALIZE3(dest, src){ float l = M_LENGHT3(src);\
   if (l > 0) { l = 1.0f / l; (dest).x = (src).x * l; (dest).y = (src).y * l; (dest).z = (src).z * l; }\
   else { (dest).x = (dest).y = (dest).z = 0.0f; }}

#define M_CROSS2(A, B) ((A).x * (B).y - (A).y * (B).x)
#define M_CROSS3(dest, A, B) (dest).x = (A).y * (B).z - (A).z * (B).y; (dest).y = (A).z * (B).x - (A).x * (B).z; (dest).z = (A).x * (B).y - (A).y * (B).x;

#define M_ADD2(dest, A, B) (dest).x = (A).x + (B).x; (dest).y = (A).y + (B).y;
#define M_ADD3(dest, A, B) M_ADD2(dest, A, B) (dest).z = (A).z + (B).z;
#define M_ADD4(dest, A, B) M_ADD3(dest, A, B) (dest).w = (A).w + (B).w;

#define M_SUB2(dest, A, B) (dest).x = (A).x - (B).x; (dest).y = (A).y - (B).y;
#define M_SUB3(dest, A, B) M_SUB2(dest, A, B) (dest).z = (A).z - (B).z;
#define M_SUB4(dest, A, B) M_SUB3(dest, A, B) (dest).w = (A).w - (B).w;

#define M_MUL2(dest, A, B) (dest).x = (A).x * (B).x; (dest).y = (A).y * (B).y;
#define M_MUL3(dest, A, B) M_MUL2(dest, A, B) (dest).z = (A).z * (B).z;
#define M_MUL4(dest, A, B) M_MUL3(dest, A, B) (dest).w = (A).w * (B).w;

#define M_DIV2(dest, A, B) (dest).x = (A).x / (B).x; (dest).y = (A).y / (B).y;
#define M_DIV3(dest, A, B) M_DIV2(dest, A, B) (dest).z = (A).z / (B).z;
#define M_DIV4(dest, A, B) M_DIV3(dest, A, B) (dest).w = (A).w / (B).w;

/* basic math */
MMAPI unsigned int m_next_power_of_two(unsigned int x);

/* rand (Marsaglia MWC generator) */
MMAPI void m_srand(unsigned int z, unsigned int w);
MMAPI unsigned int m_rand(void);
MMAPI float m_randf(void); /* (0 - 1) range */

/* interpolation */
MMAPI float m_interpolation_cubic(float y0, float y1, float y2, float y3, float mu);
MMAPI float m_interpolation_catmullrom(float y0, float y1, float y2, float y3, float mu);

/* quaternion, angles in radian */
#define M_QUAT_IDENTITY() {0.0f, 0.0f, 0.0f, 0.99999999f}

MMAPI void m_quat_identity(float4 *dest);
MMAPI void m_quat_normalize(float4 *dest, const float4 *src);
MMAPI void m_quat_rotation_axis(float4 *dest, const float3 *axis, float angle);
MMAPI void m_quat_rotation_euler(float4 *dest, const float3 *euler);
MMAPI void m_quat_mul(float4 *dest, const float4 *A, const float4 *B);
MMAPI void m_quat_slerp(float4 *dest, const float4 *A, const float4 *B, float mu);
MMAPI void m_quat_extract_axis_angle(float3 *axis, float *angle, const float4 *quat);

/* matrix 4x4 (float 16), angles in radian */
#define M_MAT4_IDENTITY() {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}

MMAPI void m_mat4_identity(float *dest);
MMAPI void m_mat4_perspective(float *dest, float fov, float ratio, float znear, float zfar);
MMAPI void m_mat4_ortho(float *dest, float left, float right, float bottom, float top, float znear, float zfar);
MMAPI void m_mat4_lookat(float *dest, const float3 *pos, const float3 *dir, const float3 *up);
MMAPI void m_mat4_translation(float *dest, const float3 *translation);
MMAPI void m_mat4_rotation_axis(float *dest, const float3 *axis, float angle);
MMAPI void m_mat4_rotation_euler(float *dest, const float3 *euler);
MMAPI void m_mat4_scale(float *dest, const float3 *scale);
MMAPI void m_mat4_mul(float *dest, const float *A, const float *B);
MMAPI void m_mat4_transpose(float *dest, const float *src);
MMAPI void m_mat4_inverse_transpose(float *dest, const float *src);
MMAPI void m_mat4_inverse(float *dest, const float *src);
MMAPI void m_mat4_extract_euler(float3 *dest, const float *matrix);
MMAPI void m_mat4_rotate3(float3 *dest, const float *matrix, const float3 *src);
MMAPI void m_mat4_inverse_rotate3(float3 *dest, const float *matrix, const float3 *src);
MMAPI void m_mat4_transform3(float3 *dest, const float *matrix, const float3 *src);
MMAPI void m_mat4_transform4(float4 *dest, const float *matrix, const float4 *src);

/* 2d */
MMAPI int   m_2d_line_to_line_intersection(float2 *dest, float2 *p11, float2 *p12, float2 *p21, float2 *p22);
MMAPI int   m_2d_box_to_box_collision(float2 *min1, float2 *max1, float2 *min2, float2 *max2);
MMAPI float m_2d_polygon_area(float2 *points, int count);
MMAPI float m_2d_polyline_length(float2 *points, int count);

/* 3d */
MMAPI int   m_3d_box_to_box_collision(float3 *min1, float3 *max1, float3 *min2, float3 *max2);
MMAPI int   m_3d_ray_box_intersection_in_out(float3 *ray_origin, float3 *ray_direction, float3 *box_min, float3 *box_max, float *in, float *out);
MMAPI int   m_3d_ray_sphere_intersection_in_out(float3 *ray_origin, float3 *ray_direction, float3 *sphere_origin, float sphere_radius2, float *in, float *out);
MMAPI int   m_3d_tri_box_overlap(float3 *box_center, float3 *box_half_size, float3 *vert1, float3 *vert2, float3 *vert3);
MMAPI float m_3d_polyline_length(float3 *points, int count);
MMAPI float m_3d_ray_plane_intersection(float3 *ray_origin, float3 *ray_direction, float3 *plane_origin, float3 *plane_normal);
MMAPI float m_3d_ray_sphere_intersection(float3 *ray_origin, float3 *ray_direction, float3 *sphere_origin, float sphere_radius2);
MMAPI float m_3d_ray_box_intersection(float3 *ray_origin, float3 *ray_direction, float3 *box_min, float3 *box_max);
MMAPI float m_3d_ray_triangle_intersection(float3 *ray_origin, float3 *ray_direction, float3 *vert1, float3 *vert2, float3 *vert3, float *u, float *v);

#ifdef __cplusplus
}
#endif
/*
----------------------------------------------------------------------*/
#endif /* M_MATH_H */

#ifdef M_MATH_IMPLEMENTATION

#ifndef __OPENCL_VERSION__
#include <math.h>
#endif

static unsigned int _m_rz = 362436069;
static unsigned int _m_rw = 521288629;

MMAPI unsigned int m_next_power_of_two(unsigned int x)
{
   if (x == 0)
      return 0;
   x--;
   x |= x >> 1;
   x |= x >> 2;
   x |= x >> 4;
   x |= x >> 8;
   x |= x >> 16;
   x++;
   return x;
}

MMAPI void m_srand(unsigned int z, unsigned int w)
{
   _m_rz = z;
   _m_rw = w;
}

MMAPI unsigned int m_rand(void)
{
   _m_rz = 36969 * (_m_rz & 65535) + (_m_rz >> 16);
   _m_rw = 18000 * (_m_rw & 65535) + (_m_rw >> 16);
   return (_m_rz << 16) + _m_rw;
}

MMAPI float m_randf(void)
{
   unsigned int u = m_rand();
   return (u + 1.0) * 2.328306435454494e-10;
}

MMAPI float m_interpolation_cubic(float y0, float y1, float y2, float y3, float mu)
{
   float a0, a1, a2, a3, mu2;
   mu2 = mu * mu;
   a0 = y3 - y2 - y0 + y1;
   a1 = y0 - y1 - a0;
   a2 = y2 - y0;
   a3 = y1;
   return (a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3);
}

MMAPI float m_interpolation_catmullrom(float y0, float y1, float y2, float y3, float mu)
{
   float a0, a1, a2, a3, mu2;
   mu2 = mu * mu;
   a0 = -0.5f * y0 + 1.5f * y1 - 1.5f * y2 + 0.5f * y3;
   a1 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
   a2 = -0.5f * y0 + 0.5f * y2;
   a3 = y1;
   return (a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3);
}

MMAPI void m_quat_identity(float4 *dest)
{
   static float4 ident = M_QUAT_IDENTITY();
   dest->x = ident.x;
   dest->y = ident.y;
   dest->z = ident.z;
   dest->w = ident.w;
}

MMAPI void m_quat_normalize(float4 *dest, const float4 *src)
{
   float l = M_LENGHT4(*src);
   if (l > 0.00000001f) {
      float m = 1.0f / l;
      dest->x = src->x * m;
      dest->y = src->y * m;
      dest->z = src->z * m;
      dest->w = src->w * m;
   }
   else {
      m_quat_identity(dest);
   }
}

MMAPI void m_quat_rotation_axis(float4 *dest, const float3 *axis, float angle)
{
   float ha = angle * 0.5f;
   float sina = sinf(ha);
   float cosa = cosf(ha);
   dest->x = sina * axis->x;
   dest->y = sina * axis->y;
   dest->z = sina * axis->z;
   dest->w = cosa;
   m_quat_normalize(dest, dest);
}

MMAPI void m_quat_rotation_euler(float4 *dest, const float3 *euler)
{
   float ti = euler->x * 0.5f;
   float tj = euler->y * 0.5f;
   float th = euler->z * 0.5f;
   float ci = cosf(ti);
   float cj = cosf(tj);
   float ch = cosf(th);
   float si = sinf(ti);
   float sj = sinf(tj);
   float sh = sinf(th);
   float cc = ci*ch;
   float cs = ci*sh;
   float sc = si*ch;
   float ss = si*sh;
   dest->x = cj*sc - sj*cs;
   dest->y = cj*ss + sj*cc;
   dest->z = cj*cs - sj*sc;
   dest->w = cj*cc + sj*ss;
}

MMAPI void m_quat_mul(float4 *dest, const float4 *A, const float4 *B)
{
   dest->x = (B->w * A->x) + (B->x * A->w) + (B->y * A->z) - (B->z * A->y);
   dest->y = (B->w * A->y) + (B->y * A->w) + (B->z * A->x) - (B->x * A->z);
   dest->z = (B->w * A->z) + (B->z * A->w) + (B->x * A->y) - (B->y * A->x);
   dest->w = (B->w * A->w) - (B->x * A->x) - (B->y * A->y) - (B->z * A->z);
}

MMAPI void m_quat_slerp(float4 *dest, const float4 *A, const float4 *B, float mu)
{
   float4 C;
   float dot = A->x * B->x + A->y * B->y + A->z * B->z + A->w * B->w;

   if (dot < 0) {
      dot = -dot;
      C.x = -B->x;
      C.y = -B->y;
      C.z = -B->z;
      C.w = -B->w;
   }
   else {
      C.x = B->x;
      C.y = B->y;
      C.z = B->z;
      C.w = B->w;
   }

   if (dot < 0.95f) {
      float a = acosf(dot);
      float f1 = sinf(a * (1.0f - mu));
      float f2 = sinf(a * mu);
      float f3 = sinf(a);
      dest->x = (A->x * f1 + C.x * f2) / f3;
      dest->y = (A->y * f1 + C.y * f2) / f3;
      dest->z = (A->z * f1 + C.z * f2) / f3;
      dest->w = (A->w * f1 + C.w * f2) / f3;
   }
   /* linear interpolation */
   else {
      float imu = 1.0f - mu;
      dest->x = A->x * imu + C.x * mu;
      dest->y = A->y * imu + C.y * mu;
      dest->z = A->z * imu + C.z * mu;
      dest->w = A->w * imu + C.w * mu;
      m_quat_normalize(dest, dest); 
   }
}

MMAPI void m_quat_extract_axis_angle(float3 *axis, float *angle, const float4 *quat)
{
   float a, s;
   float v = M_CLAMP(quat->w, -1.0f, 1.0f);

   a = acosf(v);
   s = sinf(a);

   if (s != 0) {
      float m = 1.0f / s;
      axis->x = quat->x * m;
      axis->y = quat->y * m;
      axis->z = quat->z * m;
   }
   else {
      axis->x = 0.0f;
      axis->y = 1.0f;
      axis->z = 0.0f;
   }
   *angle = a * 2.0f;
}

MMAPI void m_mat4_identity(float *dest)
{
   static float ident[16] = M_MAT4_IDENTITY(); int i;
   for (i = 0; i < 16; i++)
      dest[i] = ident[i];
}

MMAPI void m_mat4_perspective(float *dest, float fov, float ratio, float znear, float zfar)
{
   float ymax, xmax;
   float left, right, bottom, top;
   float temp, temp2, temp3, temp4;

   ymax = znear * tanf(fov);
   xmax = ymax * ratio;
    
   left = -xmax;
   right = xmax;
   bottom = -ymax;
   top = ymax;

   temp = 2.0f * znear;
   temp2 = right - left;
   temp3 = top - bottom;
   temp4 = zfar - znear;

   dest[0] = temp / temp2;
   dest[1] = 0.0f;
   dest[2] = 0.0f;
   dest[3] = 0.0f;

   dest[4] = 0.0f;
   dest[5] = temp / temp3;
   dest[6] = 0.0f;
   dest[7] = 0.0f;

   dest[8] = (right + left) / temp2;
   dest[9] = (top + bottom) / temp3;
   dest[10] = (-zfar - znear) / temp4;
   dest[11] = -1.0f;

   dest[12] = 0.0f;
   dest[13] = 0.0f;
   dest[14] = (-temp * zfar) / temp4;
   dest[15] = 0.0f;
}

MMAPI void m_mat4_ortho(float *dest, float left, float right, float bottom, float top, float znear, float zfar)
{
   if (right == left || top == bottom || zfar == znear) // invalid
      return;

   dest[0] = 2.0f / (right-left);
   dest[1] = 0.0f;
   dest[2] = 0.0f;
   dest[3] = 0.0f;

   dest[4] = 0.0f;
   dest[5] = 2.0f / (top-bottom);
   dest[6] = 0.0f;
   dest[7] = 0.0f;

   dest[8] = 0.0f;
   dest[9] = 0.0f;
   dest[10] = -2.0f / (zfar-znear);
   dest[11] = 0.0f;

   dest[12] = -(right + left) / (right - left);
   dest[13] = -(top + bottom) / (top - bottom);
   dest[14] = -(zfar + znear) / (zfar - znear);
   dest[15] = 1.0f;
}

MMAPI void m_mat4_lookat(float *dest, const float3 *pos, const float3 *dir, const float3 *up)
{
   float3 lftn, upn, dirn;

   M_CROSS3(lftn, *dir, *up);
   M_CROSS3(upn, lftn, *dir);
   M_NORMALIZE3(lftn, lftn);
   M_NORMALIZE3(upn, upn);
   M_NORMALIZE3(dirn, *dir);

   dest[0] = lftn.x;
   dest[1] = upn.x;
   dest[2] = -dirn.x;
   dest[3] = 0.0f;

   dest[4] = lftn.y;
   dest[5] = upn.y;
   dest[6] = -dirn.y;
   dest[7] = 0.0f;

   dest[8] = lftn.z;
   dest[9] = upn.z;
   dest[10] = -dirn.z;
   dest[11] = 0.0f;

   dest[12] = -M_DOT3(lftn, *pos);
   dest[13] = -M_DOT3(upn, *pos);
   dest[14] = M_DOT3(dirn, *pos);
   dest[15] = 1.0f;
}

MMAPI void m_mat4_translation(float *dest, const float3 *translation)
{
   dest[12] = translation->x;
   dest[13] = translation->y;
   dest[14] = translation->z;
}

MMAPI void m_mat4_rotation_axis(float *dest, const float3 *axis, float angle)
{
   float sina = sinf(angle);
   float cosa = cosf(angle);
   float icosa = 1.0f - cosa;

   dest[0] = axis->x * axis->x + cosa * (1.0f - axis->x * axis->x);
   dest[1] = axis->x * axis->y * icosa + sina * axis->z;
   dest[2] = axis->x * axis->z * icosa - sina * axis->y;

   dest[4] = axis->x * axis->y * icosa - sina * axis->z;
   dest[5] = axis->y * axis->y + cosa * (1.0f - axis->y * axis->y);
   dest[6] = axis->y * axis->z * icosa + sina * axis->x;

   dest[8] = axis->x * axis->z * icosa + sina * axis->y;
   dest[9] = axis->y * axis->z * icosa - sina * axis->x;
   dest[10] = axis->z * axis->z + cosa * (1.0f - axis->z * axis->z);
}

MMAPI void m_mat4_rotation_euler(float *dest, const float3 *euler)
{
   float cr = cosf(euler->x);
   float sr = sinf(euler->x);
   float cp = cosf(euler->y);
   float sp = sinf(euler->y);
   float cy = cosf(euler->z);
   float sy = sinf(euler->z);
   float srsp = sr * sp;
   float crsp = cr * sp;

   dest[0] = cp * cy;
   dest[1] = cp * sy;
   dest[2] = -sp;

   dest[4] = srsp * cy - cr * sy;
   dest[5] = srsp * sy + cr * cy;
   dest[6] = sr * cp;

   dest[8] = crsp * cy + sr * sy;
   dest[9] = crsp * sy - sr * cy;
   dest[10]= cr * cp;
}

MMAPI void m_mat4_scale(float *dest, const float3 *scale)
{
   dest[0] = scale->x;
   dest[5] = scale->y;
   dest[10] = scale->z;
}

MMAPI void m_mat4_mul(float *dest, const float *A, const float *B)
{
   dest[0] = A[0] * B[0] + A[4] * B[1] + A[8] * B[2] + A[12] * B[3];
   dest[1] = A[1] * B[0] + A[5] * B[1] + A[9] * B[2] + A[13] * B[3];
   dest[2] = A[2] * B[0] + A[6] * B[1] + A[10] * B[2] + A[14] * B[3];
   dest[3] = A[3] * B[0] + A[7] * B[1] + A[11] * B[2] + A[15] * B[3];
   dest[4] = A[0] * B[4] + A[4] * B[5] + A[8] * B[6] + A[12] * B[7];
   dest[5] = A[1] * B[4] + A[5] * B[5] + A[9] * B[6] + A[13] * B[7];
   dest[6] = A[2] * B[4] + A[6] * B[5] + A[10] * B[6] + A[14] * B[7];
   dest[7] = A[3] * B[4] + A[7] * B[5] + A[11] * B[6] + A[15] * B[7];
   dest[8] = A[0] * B[8] + A[4] * B[9] + A[8] * B[10] + A[12] * B[11];
   dest[9] = A[1] * B[8] + A[5] * B[9] + A[9] * B[10] + A[13] * B[11];
   dest[10] = A[2] * B[8] + A[6] * B[9] + A[10] * B[10] + A[14] * B[11];
   dest[11] = A[3] * B[8] + A[7] * B[9] + A[11] * B[10] + A[15] * B[11];
   dest[12] = A[0] * B[12] + A[4] * B[13] + A[8] * B[14] + A[12] * B[15];
   dest[13] = A[1] * B[12] + A[5] * B[13] + A[9] * B[14] + A[13] * B[15];
   dest[14] = A[2] * B[12] + A[6] * B[13] + A[10] * B[14] + A[14] * B[15];
   dest[15] = A[3] * B[12] + A[7] * B[13] + A[11] * B[14] + A[15] * B[15];
}

MMAPI void m_mat4_transpose(float *dest, const float *src)
{
   dest[0] =  src[0]; dest[1] =  src[4]; dest[2] =  src[8];  dest[3] =  src[12];
   dest[4] =  src[1]; dest[5] =  src[5]; dest[6] =  src[9];  dest[7] =  src[13];
   dest[8] =  src[2]; dest[9] =  src[6]; dest[10] = src[10]; dest[11] = src[14];
   dest[12] = src[3]; dest[13] = src[7]; dest[14] = src[11]; dest[15] = src[15];
}

MMAPI void m_mat4_inverse_transpose(float *dest, const float *src)
{
   float tmp[12];
   float det;

   // calculate pairs for first 8 elements (cofactors)
   tmp[0] =  src[10] * src[15];
   tmp[1] =  src[11] * src[14];
   tmp[2] =  src[9] *  src[15];
   tmp[3] =  src[11] * src[13];
   tmp[4] =  src[9] *  src[14];
   tmp[5] =  src[10] * src[13];
   tmp[6] =  src[8] *  src[15];
   tmp[7] =  src[11] * src[12];
   tmp[8] =  src[8] *  src[14];
   tmp[9] =  src[10] * src[12];
   tmp[10] = src[8] *  src[13];
   tmp[11] = src[9] *  src[12];

   // calculate first 8 elements (cofactors)
   dest[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7]
           - tmp[1]*src[5] - tmp[2]*src[6] - tmp[5]*src[7];

   dest[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7]
            -tmp[0]*src[4] - tmp[7]*src[6] - tmp[8]*src[7];

   dest[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7]
            -tmp[3]*src[4] - tmp[6]*src[5] - tmp[11]*src[7];

   dest[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6]
            -tmp[4]*src[4] - tmp[9]*src[5] - tmp[10]*src[6];

   dest[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3]
            -tmp[0]*src[1] - tmp[3]*src[2] - tmp[4]*src[3];

   dest[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3]
            -tmp[1]*src[0] - tmp[6]*src[2] - tmp[9]*src[3];

   dest[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3]
            -tmp[2]*src[0] - tmp[7]*src[1] - tmp[10]*src[3];

   dest[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2]
            -tmp[5]*src[0] - tmp[8]*src[1] - tmp[11]*src[2];

   // calculate pairs for second 8 elements (cofactors)
   tmp[0] =  src[2] * src[7];
   tmp[1] =  src[3] * src[6];
   tmp[2] =  src[1] * src[7];
   tmp[3] =  src[3] * src[5];
   tmp[4] =  src[1] * src[6];
   tmp[5] =  src[2] * src[5];
   tmp[6] =  src[0] * src[7];
   tmp[7] =  src[3] * src[4];
   tmp[8] =  src[0] * src[6];
   tmp[9] =  src[2] * src[4];
   tmp[10] = src[0] * src[5];
   tmp[11] = src[1] * src[4];

   // calculate second 8 elements (cofactors)
   dest[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15]
            -tmp[1]*src[13] - tmp[2]*src[14] - tmp[5]*src[15];

   dest[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15]
            -tmp[0]*src[12] - tmp[7]*src[14] - tmp[8]*src[15];

   dest[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15]
             -tmp[3]*src[12] - tmp[6]*src[13] - tmp[11]*src[15];

   dest[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14]
             -tmp[4]*src[12] - tmp[9]*src[13] - tmp[10]*src[14];

   dest[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9]
             -tmp[4]*src[11] - tmp[0]*src[9] - tmp[3]*src[10];

   dest[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10]
             -tmp[6]*src[10] - tmp[9]*src[11] - tmp[1]*src[8];

   dest[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8]
             -tmp[10]*src[11] - tmp[2]*src[8] - tmp[7]*src[9];

   dest[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9]
             -tmp[8]*src[9] - tmp[11]*src[10] - tmp[5]*src[8];

   // calculate determinant
   det = src[0] * dest[0]
       + src[1] * dest[1]
       + src[2] * dest[2]
       + src[3] * dest[3];

   if (det == 0.0f) {
      m_mat4_identity(dest);
   }
   else {
      float m = 1.0f / det; int i;
      for (i = 0; i < 16; i++)
         dest[i] *= m;
   }
}

MMAPI void m_mat4_inverse(float *dest, const float *src)
{
   float tmp[16];
   m_mat4_inverse_transpose(tmp, src);
   m_mat4_transpose(dest, tmp);
}

MMAPI void m_mat4_extract_euler(float3 *dest, const float *matrix)
{
   float cy = sqrtf(matrix[0] * matrix[0] + matrix[1] * matrix[1]);
   if (cy > (16.0 * 1.192092896e-07F)) {

      float3 euler1;
      float3 euler2;

      euler1.x = atan2f( matrix[6], matrix[10]);
      euler1.y = atan2f(-matrix[2], cy);
      euler1.z = atan2f( matrix[1], matrix[0]);
      
      euler2.x = atan2f(-matrix[6], -matrix[10]);
      euler2.y = atan2f(-matrix[2], -cy);
      euler2.z = atan2f(-matrix[1], -matrix[0]);
      
      if ((M_ABS(euler1.x) + M_ABS(euler1.y) + M_ABS(euler1.z)) > 
          (M_ABS(euler2.x) + M_ABS(euler2.y) + M_ABS(euler2.z)))
      {
         dest->x = euler2.x;
         dest->y = euler2.y;
         dest->z = euler2.z;
      }
      else
      {
         dest->x = euler1.x;
         dest->y = euler1.y;
         dest->z = euler1.z;
      }
   }
   else {
      dest->x = atan2f(-matrix[9], matrix[5]);
      dest->y = atan2f(-matrix[2], cy);
      dest->z = 0.0f;
   }
}

MMAPI void m_mat4_rotate3(float3 *dest, const float *matrix, const float3 *src)
{
   dest->x = matrix[0] * src->x + matrix[4] * src->y + matrix[8] * src->z;
   dest->y = matrix[1] * src->x + matrix[5] * src->y + matrix[9] * src->z;
   dest->z = matrix[2] * src->x + matrix[6] * src->y + matrix[10] * src->z;
}

MMAPI void m_mat4_inverse_rotate3(float3 *dest, const float *matrix, const float3 *src)
{
   dest->x = matrix[0] * src->x + matrix[1] * src->y + matrix[2] * src->z;
   dest->y = matrix[4] * src->x + matrix[5] * src->y + matrix[6] * src->z;
   dest->z = matrix[8] * src->x + matrix[9] * src->y + matrix[10] * src->z;
}

MMAPI void m_mat4_transform3(float3 *dest, const float *matrix, const float3 *src)
{
   dest->x = matrix[0] * src->x + matrix[4] * src->y + matrix[8] * src->z + matrix[12];
   dest->y = matrix[1] * src->x + matrix[5] * src->y + matrix[9] * src->z + matrix[13];
   dest->z = matrix[2] * src->x + matrix[6] * src->y + matrix[10] * src->z + matrix[14];
}

MMAPI void m_mat4_transform4(float4 *dest, const float *matrix, const float4 *src)
{
   dest->x = matrix[0] * src->x + matrix[4] * src->y + matrix[8] * src->z + matrix[12] * src->w;
   dest->y = matrix[1] * src->x + matrix[5] * src->y + matrix[9] * src->z + matrix[13] * src->w;
   dest->z = matrix[2] * src->x + matrix[6] * src->y + matrix[10] * src->z + matrix[14] * src->w;
   dest->w = matrix[3] * src->x + matrix[7] * src->y + matrix[11] * src->z + matrix[15] * src->w;
}

MMAPI float m_2d_polygon_area(float2 *points, int count)
{
   float fx, fy, a; int p;
   if (count < 3)
      return 0;

   fx = points[count - 1].x * points[0].y;
   fy = points[count - 1].y * points[0].x;

   for (p = 1; p < count; p++) {
      fx += points[p - 1].x * points[p].y;
      fy += points[p - 1].y * points[p].x;
   }

   a = (fx - fy) * 0.5f;
   return M_ABS(a);
}

MMAPI float m_2d_polyline_length(float2 *points, int count)
{
   float l = 0; int p;
   for (p = 1; p < count; p++) {
      float2 v; M_SUB2(v, points[p], points[p - 1]);
      l += M_LENGHT2(v);
   }
   return l;
}

MMAPI int m_2d_line_to_line_intersection(float2 *dest, float2 *p11, float2 *p12, float2 *p21, float2 *p22)
{
   float2 DP, QA, QB;
   float d, la, lb;

   DP.x = p21->x - p11->x ; DP.y = p21->y - p11->y;
   QA.x = p12->x - p11->x ; QA.y = p12->y - p11->y;
   QB.x = p22->x - p21->x ; QB.y = p22->y - p21->y;

   d = QA.y * QB.x - QB.y * QA.x;
   if (d == 0)
      return 0;
      
   la = (QB.x * DP.y - QB.y * DP.x) / d;
   if (la < 0 || la > 1)
      return 0;

   lb = (QA.x * DP.y - QA.y * DP.x) / d;
   if (lb < 0 || lb > 1)
      return 0;

   dest->x = p11->x + la * QA.x;
   dest->y = p11->y + la * QA.y;
   return 1;
}

MMAPI int m_2d_box_to_box_collision(float2 *min1, float2 *max1, float2 *min2, float2 *max2)
{
   return !(
   (min1->x > max2->x) || (max1->x < min2->x) ||
   (min1->y > max2->y) || (max1->y < min2->y));
}

MMAPI float m_3d_polyline_length(float3 *points, int count)
{
   float l = 0; int p;
   for (p = 1; p < count; p++) {
      float3 v; M_ADD3(v, points[p], points[p - 1]);
      l += M_LENGHT3(v);
   }
   return l;
}

MMAPI int m_3d_box_to_box_collision(float3 *min1, float3 *max1, float3 *min2, float3 *max2)
{
   return !(
   (min1->x > max2->x) || (max1->x < min2->x) ||
   (min1->y > max2->y) || (max1->y < min2->y) ||
   (min1->z > max2->z) || (max1->z < min2->z));
}

MMAPI int m_3d_ray_box_intersection_in_out(float3 *ray_origin, float3 *ray_direction, float3 *box_min, float3 *box_max, float *in, float *out)
{
   float3 idir;
   float3 tmin, tmax;
   float3 tnear, tfar;
   float tnx, tny, tfy;
   float _in, _out;

   idir.x = 1.0f / ray_direction->x;
   idir.y = 1.0f / ray_direction->y;
   idir.z = 1.0f / ray_direction->z;
   tmin.x = (box_min->x - ray_origin->x) * idir.x;
   tmin.y = (box_min->y - ray_origin->y) * idir.y;
   tmin.z = (box_min->z - ray_origin->z) * idir.z;
   tmax.x = (box_max->x - ray_origin->x) * idir.x;
   tmax.y = (box_max->y - ray_origin->y) * idir.y;
   tmax.z = (box_max->z - ray_origin->z) * idir.z;

   M_MIN3(tnear, tmin, tmax);
   M_MAX3(tfar,  tmin, tmax);

   tnx = M_MAX(tnear.x, 0.0f);
   tny = M_MAX(tnear.y, tnear.z);
   tfy = M_MIN(tfar.y, tfar.z);

   _in = M_MAX(tnx, tny);
   _out = M_MIN(tfar.x, tfy);
    
   if (_out > 0.0f && _in < _out) {
      *in = _in;
      *out = _out;
      return 1;
   }
   return 0;
}

MMAPI int m_3d_ray_sphere_intersection_in_out(float3 *ray_origin, float3 *ray_direction, float3 *sphere_origin, float sphere_radius2, float *in, float *out)
{
   float3 vec;
   float b, det;

   M_SUB3(vec, *sphere_origin, *ray_origin);
   b = M_DOT3(vec, *ray_direction);
   det = b * b - M_DOT3(vec, vec) + sphere_radius2;

   if (det < 0) return 0;
   else det = sqrtf(det);
   *in = b - det;
   *out = b + det;
   return 1;
}

MMAPI float m_3d_ray_plane_intersection(float3 *ray_origin, float3 *ray_direction, float3 *plane_origin, float3 *plane_normal)
{
   float constant = -M_DOT3(*plane_normal, *plane_origin);
   float normal_dot = M_DOT3(*plane_normal, *ray_direction);
   float plane_dist = M_DOT3(*plane_normal, *ray_origin) + constant;
   return -plane_dist / normal_dot;
}

MMAPI float m_3d_ray_sphere_intersection(float3 *ray_origin, float3 *ray_direction, float3 *sphere_origin, float sphere_radius2)
{
   float3 vec;
   float b, det;

   M_SUB3(vec, *sphere_origin, *ray_origin);
   b = M_DOT3(vec, *ray_direction);
   det = b * b - M_DOT3(vec, vec) + sphere_radius2;

   if (det < 0) return 0;
   else det = sqrtf(det);
   return b - det;
}

/* Ray-box intersection using IEEE numerical properties to ensure that the
   test is both robust and efficient, as described in:
   Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
   "An Efficient and Robust Ray-Box Intersection Algorithm"
   Journal of graphics tools, 10(1):49-54, 2005
*/
MMAPI float m_3d_ray_box_intersection(float3 *ray_origin, float3 *ray_direction, float3 *box_min, float3 *box_max)
{
   float tmin, tmax, tymin, tymax, tzmin, tzmax;
   float3 parameters[2];
   float3 inv_direction = {
      1 / ray_direction->x,
      1 / ray_direction->y,
      1 / ray_direction->z
   };
   int sign[3];

   parameters[0] = *box_min;
   parameters[1] = *box_max;

   sign[0] = (inv_direction.x < 0);
   sign[1] = (inv_direction.y < 0);
   sign[2] = (inv_direction.z < 0);

   tmin =  (parameters[sign[0]].x - ray_origin->x) * inv_direction.x;
   tymin = (parameters[sign[1]].y - ray_origin->y) * inv_direction.y;
   tmax =  (parameters[1 - sign[0]].x - ray_origin->x) * inv_direction.x;
   tymax = (parameters[1 - sign[1]].y - ray_origin->y) * inv_direction.y;
   
   if ((tmin > tymax) || (tymin > tmax))
      return 0;
  
   if (tymin > tmin) tmin = tymin;
   if (tymax < tmax) tmax = tymax;

   tzmin = (parameters[sign[2]].z - ray_origin->z) * inv_direction.z;
   tzmax = (parameters[1 - sign[2]].z - ray_origin->z) * inv_direction.z;

   if ((tmin > tzmax) || (tzmin > tmax))
      return 0;
      
   if (tzmin > tmin) tmin = tzmin;
   if (tzmax < tmax) tmax = tzmax;

   return tmin;
}

MMAPI float m_3d_ray_triangle_intersection(float3 *ray_origin, float3 *ray_direction, float3 *vert1, float3 *vert2, float3 *vert3, float *u, float *v)
{
   float3 edge1, edge2;
   float3 pvec, tvec, qvec;
   float det, inv_det;

   M_SUB3(edge1, *vert2, *vert1);
   M_SUB3(edge2, *vert3, *vert1);
   M_CROSS3(pvec, *ray_direction, edge2);

   det = M_DOT3(edge1, pvec);
   if (det == 0)
      return 0;
      
   inv_det = 1.0f / det;
   M_SUB3(tvec, *ray_origin, *vert1);

   /* u parameter */
   *u = M_DOT3(tvec, pvec) * inv_det;
   if (*u < 0.0f || *u > 1.0f)
      return 0;
      
   M_CROSS3(qvec, tvec, edge1);
   
   /* v parameter */
   *v = M_DOT3(*ray_direction, qvec) * inv_det;
   
   /* inverted comparison (to catch NaNs) */
   if (*v >= 0.0f && ((*u) + (*v)) <= 1.0f)
      return M_DOT3(edge2, qvec) * inv_det;

   return 0;
}

/********************************************************/
/* AABB-triangle overlap test code                      */
/* by Tomas Akenine-Möller                              */
/* Function: int triBoxOverlap(float boxcenter[3],      */
/*          float boxhalfsize[3],float triverts[3][3]); */
/* History:                                             */
/*   2001-03-05: released the code in its first version */
/*   2001-06-18: changed the order of the tests, faster */
/*                                                      */
/* Acknowledgement: Many thanks to Pierre Terdiman for  */
/* suggestions and discussions on how to optimize code. */
/* Thanks to David Hunt for finding a ">="-bug!         */
/********************************************************/

#ifndef M_FIND_MIN_MAX_3
#define M_FIND_MIN_MAX_3(x0, x1, x2, min, max)\
   min = max = x0;\
   if (x1 < min) min=x1;\
   if (x1 > max) max=x1;\
   if (x2 < min) min=x2;\
   if (x2 > max) max=x2;
#endif

static int _m_plane_box_overlap(float3 *normal, float3 *vert, float3 *maxbox)
{
   float3 vmin, vmax;
   if (normal->x > 0.0f) { vmin.x = -maxbox->x - vert->x; vmax.x =  maxbox->x - vert->x; }
   else                  { vmin.x =  maxbox->x - vert->x; vmax.x = -maxbox->x - vert->x; }
   if (normal->y > 0.0f) { vmin.y = -maxbox->y - vert->y; vmax.y =  maxbox->y - vert->y; }
   else                  { vmin.y =  maxbox->y - vert->y; vmax.y = -maxbox->y - vert->y; }
   if (normal->z > 0.0f) { vmin.z = -maxbox->z - vert->z; vmax.z =  maxbox->z - vert->z; }
   else                  { vmin.z =  maxbox->z - vert->z; vmax.z = -maxbox->z - vert->z; }
   if (M_DOT3(*normal, vmin) > 0.0f) return 0;
   if (M_DOT3(*normal, vmax) >= 0.0f) return 1;
   return 0;
}

/*======================== X-tests ========================*/
#define _M_AXISTEST_X01(a, b, fa, fb)\
   p0 = a*v0.y - b*v0.z;\
   p2 = a*v2.y - b*v2.z;\
   if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;}\
   rad = fa * boxhalfsize->y + fb * boxhalfsize->z;\
   if(min>rad || max<-rad) return 0;

#define _M_AXISTEST_X2(a, b, fa, fb)\
   p0 = a*v0.y - b*v0.z;\
   p1 = a*v1.y - b*v1.z;\
   if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;}\
   rad = fa * boxhalfsize->y + fb * boxhalfsize->z;\
   if(min>rad || max<-rad) return 0;

/*======================== Y-tests ========================*/
#define _M_AXISTEST_Y02(a, b, fa, fb)\
   p0 = -a*v0.x + b*v0.z;\
   p2 = -a*v2.x + b*v2.z;\
   if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;}\
   rad = fa * boxhalfsize->x + fb * boxhalfsize->z;\
   if(min>rad || max<-rad) return 0;

#define _M_AXISTEST_Y1(a, b, fa, fb)\
   p0 = -a*v0.x + b*v0.z;\
   p1 = -a*v1.x + b*v1.z;\
   if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;}\
   rad = fa * boxhalfsize->x + fb * boxhalfsize->z;\
   if(min>rad || max<-rad) return 0;

/*======================== Z-tests ========================*/
#define _M_AXISTEST_Z12(a, b, fa, fb)\
   p1 = a*v1.x - b*v1.y;\
   p2 = a*v2.x - b*v2.y;\
   if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;}\
   rad = fa * boxhalfsize->x + fb * boxhalfsize->y;\
   if(min>rad || max<-rad) return 0;

#define _M_AXISTEST_Z0(a, b, fa, fb)\
   p0 = a*v0.x - b*v0.y;\
   p1 = a*v1.x - b*v1.y;\
   if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;}\
   rad = fa * boxhalfsize->x + fb * boxhalfsize->y;\
   if(min>rad || max<-rad) return 0;

MMAPI int m_3d_tri_box_overlap(float3 *boxcenter, float3 *boxhalfsize, float3 *vert1, float3 *vert2, float3 *vert3)
{
   /*    use separating axis theorem to test overlap between triangle and box */
   /*    need to test for overlap in these directions: */
   /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
   /*       we do not even need to test these) */
   /*    2) normal of the triangle */
   /*    3) crossproduct(edge from tri, {x,y,z}-directin) */
   /*       this gives 3x3=9 more tests */

   float3 v0, v1, v2;
   float3 normal, e0, e1, e2;
   float min, max, p0, p1, p2, rad, fex, fey, fez;

   /* This is the fastest branch on Sun */
   /* move everything so that the boxcenter is in (0,0,0) */
   M_SUB3(v0, *vert1, *boxcenter);
   M_SUB3(v1, *vert2, *boxcenter);
   M_SUB3(v2, *vert3, *boxcenter);

   /* compute triangle edges */
   M_SUB3(e0, v1, v0);      /* tri edge 0 */
   M_SUB3(e1, v2, v1);      /* tri edge 1 */
   M_SUB3(e2, v0, v2);      /* tri edge 2 */

   /* Bullet 3:  */
   /*  test the 9 tests first (this was faster) */
   fex = M_ABS(e0.x);
   fey = M_ABS(e0.y);
   fez = M_ABS(e0.z);
   _M_AXISTEST_X01(e0.z, e0.y, fez, fey);
   _M_AXISTEST_Y02(e0.z, e0.x, fez, fex);
   _M_AXISTEST_Z12(e0.y, e0.x, fey, fex);

   fex = M_ABS(e1.x);
   fey = M_ABS(e1.y);
   fez = M_ABS(e1.z);
   _M_AXISTEST_X01(e1.z, e1.y, fez, fey);
   _M_AXISTEST_Y02(e1.z, e1.x, fez, fex);
   _M_AXISTEST_Z0(e1.y, e1.x, fey, fex);

   fex = M_ABS(e2.x);
   fey = M_ABS(e2.y);
   fez = M_ABS(e2.z);
   _M_AXISTEST_X2(e2.z, e2.y, fez, fey);
   _M_AXISTEST_Y1(e2.z, e2.x, fez, fex);
   _M_AXISTEST_Z12(e2.y, e2.x, fey, fex);

   /* Bullet 1: */
   /*  first test overlap in the {x,y,z}-directions */
   /*  find min, max of the triangle each direction, and test for overlap in */
   /*  that direction -- this is equivalent to testing a minimal AABB around */
   /*  the triangle against the AABB */

   /* test in X-direction */
   M_FIND_MIN_MAX_3(v0.x, v1.x, v2.x, min, max);
   if (min > boxhalfsize->x || max < -boxhalfsize->x) return 0;

   /* test in Y-direction */
   M_FIND_MIN_MAX_3(v0.y, v1.y, v2.y, min, max);
   if (min > boxhalfsize->y || max < -boxhalfsize->y) return 0;

   /* test in Z-direction */
   M_FIND_MIN_MAX_3(v0.z, v1.z, v2.z, min, max);
   if (min > boxhalfsize->z || max < -boxhalfsize->z) return 0;

   /* Bullet 2: */
   /*  test if the box intersects the plane of the triangle */
   /*  compute plane equation of triangle: normal*x+d=0 */
   M_CROSS3(normal, e0, e1);
   if (! _m_plane_box_overlap(&normal, &v0, boxhalfsize)) return 0;

   return 1;   /* box and triangle overlaps */
}

#endif /* M_MATH_IMPLEMENTATION */