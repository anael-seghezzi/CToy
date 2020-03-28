/*======================================================================
    Maratis Tiny C Library
    version 1.0
------------------------------------------------------------------------
    Copyright (c) 2015 Anael Seghezzi <www.maratis3d.com>

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

#include "ctoy.c"

#ifdef WIN32
#include <io.h>
#else
#include <dlfcn.h>
#endif

#include <dirent.h>
#include <fcntl.h>
#include <stdarg.h>
#include <libtcc.h>

/* custom calls */
void (*ctoy_begin)(void);
void (*ctoy_main_loop)(void);
void (*ctoy_end)(void);

/* system */
static char ctoy__dir[256];
TCCState *  ctoy__tcc;
int         ctoy__src_update = 0;
int         ctoy__src_state = 0;

#ifndef CTOY_PLAYER
/* dynamic refresh */
#define CTOY_MAX_SRC 256
char    ctoy__src_path[CTOY_MAX_SRC][256];
long    ctoy__src_t[CTOY_MAX_SRC];
int     ctoy__src_count = 0;
thrd_t  ctoy__src_thread_id = 0;
int     ctoy__src_thread_run = 1;

int ctoy__io_open(const char *filename, int access, ...)
{
   va_list args;
   va_start(args, access);

   // custom io (tcc HACK to get the list of active src files)
   if (strncmp(filename, "src", 3) == 0 && ctoy__src_count < CTOY_MAX_SRC) {
      strcpy(ctoy__src_path[ctoy__src_count], filename);
      ctoy__src_count++;
   }

   return open(filename, access, args);
}

size_t ctoy__io_read(int fh, void *dst, size_t max)
{
   return (size_t)read(fh, dst, max);
}

off_t ctoy__io_lseek(int fh, off_t offset, int origin)
{
   return (off_t)lseek(fh, offset, origin);
}

int ctoy__io_close(int fh)
{
   return close(fh);
}

void ctoy__io_replace(void)
{
   tcc_set_io_open(ctoy__io_open);
   tcc_set_io_read(ctoy__io_read);
   tcc_set_io_lseek(ctoy__io_lseek);
   tcc_set_io_close(ctoy__io_close);
}

long ctoy__file_t(const char *filename)
{
   struct stat buffer;
   if (stat(filename, &buffer) == 0)
      return (long)buffer.st_mtime;
   return 0;
}

int ctoy__src_thread(void * data)
{
   struct timespec t; int i;
   t.tv_sec = 0;
   t.tv_nsec = 1000000;

   while (ctoy__src_thread_run) {

      if (! ctoy__src_update)
      for (i = 0; i < ctoy__src_count; i++) {
         if (ctoy__file_t(ctoy__src_path[i]) != ctoy__src_t[i]) {
            ctoy__src_update = 1;
            break;
         }
      }

      if (ctoy__src_thread_run)
         thrd_sleep(&t, NULL);
   }

    return 0;
}

int ctoy__src_thread_init(void)
{
   ctoy__src_thread_run = 1;
   if (thrd_create(&ctoy__src_thread_id, ctoy__src_thread, NULL) != thrd_success)
      return 0;
   return 1;
}

void ctoy__src_thread_destroy(void)
{
   ctoy__src_thread_run = 0;
   thrd_join(ctoy__src_thread_id, NULL);
}

void ctoy__src_thread_restart(void)
{
   int i;
   ctoy__src_thread_destroy();
   ctoy__src_update = 0;
   for (i = 0; i < ctoy__src_count; i++)
      ctoy__src_t[i] = ctoy__file_t(ctoy__src_path[i]);
   ctoy__src_thread_init();
}
#endif /* /ifndef CTOY_PLAYER */


int ctoy__is_directory(const char * filename)
{
    DIR *pdir = opendir(filename);
   if(! pdir)
      return 0;
    closedir(pdir);
   return 1;
}

void ctoy__system_symbols(void)
{
#ifdef WIN32
   /* dirent port */
   tcc_add_symbol(ctoy__tcc, "opendir", opendir);
   tcc_add_symbol(ctoy__tcc, "closedir", closedir);
   tcc_add_symbol(ctoy__tcc, "readdir", readdir);
   tcc_add_symbol(ctoy__tcc, "rewinddir", rewinddir);
#endif
}

void ctoy__gles2_symbols(void)
{
   static const char *sym[142] = {
   "glActiveTexture", "glAttachShader", "glBindAttribLocation", "glBindBuffer", "glBindFramebuffer", "glBindRenderbuffer", "glBindTexture", "glBlendColor",
   "glBlendEquation", "glBlendEquationSeparate", "glBlendFunc", "glBlendFuncSeparate", "glBufferData", "glBufferSubData", "glCheckFramebufferStatus", "glClear", 
   "glClearColor", "glClearDepthf", "glClearStencil", "glColorMask", "glCompileShader", "glCompressedTexImage2D", "glCompressedTexSubImage2D", "glCopyTexImage2D", 
   "glCopyTexSubImage2D", "glCreateProgram", "glCreateShader", "glCullFace", "glDeleteBuffers", "glDeleteFramebuffers", "glDeleteProgram", "glDeleteRenderbuffers", 
   "glDeleteShader", "glDeleteTextures", "glDepthFunc", "glDepthMask", "glDepthRangef", "glDetachShader", "glDisable", "glDisableVertexAttribArray", 
   "glDrawArrays", "glDrawElements", "glEnable", "glEnableVertexAttribArray", "glFinish", "glFlush", "glFramebufferRenderbuffer", "glFramebufferTexture2D", 
   "glFrontFace", "glGenBuffers", "glGenerateMipmap", "glGenFramebuffers", "glGenRenderbuffers", "glGenTextures", "glGetActiveAttrib", "glGetActiveUniform", 
   "glGetAttachedShaders", "glGetAttribLocation", "glGetBooleanv", "glGetBufferParameteriv", "glGetError", "glGetFloatv", "glGetFramebufferAttachmentParameteriv", "glGetIntegerv", 
   "glGetProgramiv", "glGetProgramInfoLog", "glGetRenderbufferParameteriv", "glGetShaderiv", "glGetShaderInfoLog", "glGetShaderPrecisionFormat", "glGetShaderSource", "glGetString", 
   "glGetTexParameterfv", "glGetTexParameteriv", "glGetUniformfv", "glGetUniformiv", "glGetUniformLocation", "glGetVertexAttribfv", "glGetVertexAttribiv", "glGetVertexAttribPointerv", 
   "glHint", "glIsBuffer", "glIsEnabled", "glIsFramebuffer", "glIsProgram", "glIsRenderbuffer", "glIsShader", "glIsTexture", 
   "glLineWidth", "glLinkProgram", "glPixelStorei", "glPolygonOffset", "glReadPixels", "glReleaseShaderCompiler", "glRenderbufferStorage", "glSampleCoverage", 
   "glScissor", "glShaderBinary", "glShaderSource", "glStencilFunc", "glStencilFuncSeparate", "glStencilMask", "glStencilMaskSeparate", "glStencilOp", 
   "glStencilOpSeparate", "glTexImage2D", "glTexParameterf", "glTexParameterfv", "glTexParameteri", "glTexParameteriv", "glTexSubImage2D", "glUniform1f", 
   "glUniform1fv", "glUniform1i", "glUniform1iv", "glUniform2f", "glUniform2fv", "glUniform2i", "glUniform2iv", "glUniform3f", 
   "glUniform3fv", "glUniform3i", "glUniform3iv", "glUniform4f", "glUniform4fv", "glUniform4i", "glUniform4iv", "glUniformMatrix2fv", 
   "glUniformMatrix3fv", "glUniformMatrix4fv", "glUseProgram", "glValidateProgram", "glVertexAttrib1f", "glVertexAttrib1fv", "glVertexAttrib2f", "glVertexAttrib2fv", 
   "glVertexAttrib3f", "glVertexAttrib3fv", "glVertexAttrib4f", "glVertexAttrib4fv", "glVertexAttribPointer", "glViewport",
   }; int i;

   for (i = 0; i < 142; i++) {
      GLFWglproc fun = glfwGetProcAddress(sym[i]);
      if (fun) tcc_add_symbol(ctoy__tcc, sym[i], fun);
   }
}

void ctoy__symbols(void)
{
   tcc_add_symbol(ctoy__tcc, "ctoy_t", ctoy_t);
   tcc_add_symbol(ctoy__tcc, "ctoy_window_width", ctoy_window_width);
   tcc_add_symbol(ctoy__tcc, "ctoy_window_height", ctoy_window_height);
   tcc_add_symbol(ctoy__tcc, "ctoy_frame_buffer_width", ctoy_frame_buffer_width);
   tcc_add_symbol(ctoy__tcc, "ctoy_frame_buffer_height", ctoy_frame_buffer_height);
   tcc_add_symbol(ctoy__tcc, "ctoy_mouse_x", ctoy_mouse_x);
   tcc_add_symbol(ctoy__tcc, "ctoy_mouse_y", ctoy_mouse_y);
   tcc_add_symbol(ctoy__tcc, "ctoy_scroll_x", ctoy_scroll_x);
   tcc_add_symbol(ctoy__tcc, "ctoy_scroll_y", ctoy_scroll_y);
   tcc_add_symbol(ctoy__tcc, "ctoy_get_pen_data", ctoy_get_pen_data);
   tcc_add_symbol(ctoy__tcc, "ctoy_get_time", ctoy_get_time);
   tcc_add_symbol(ctoy__tcc, "ctoy_sleep", ctoy_sleep);
   tcc_add_symbol(ctoy__tcc, "ctoy_key_press", ctoy_key_press);
   tcc_add_symbol(ctoy__tcc, "ctoy_key_release", ctoy_key_release);
   tcc_add_symbol(ctoy__tcc, "ctoy_key_pressed", ctoy_key_pressed);
   tcc_add_symbol(ctoy__tcc, "ctoy_mouse_button_press", ctoy_mouse_button_press);
   tcc_add_symbol(ctoy__tcc, "ctoy_mouse_button_release", ctoy_mouse_button_release);
   tcc_add_symbol(ctoy__tcc, "ctoy_mouse_button_pressed", ctoy_mouse_button_pressed);

   tcc_add_symbol(ctoy__tcc, "ctoy_joystick_present", ctoy_joystick_present);
   tcc_add_symbol(ctoy__tcc, "ctoy_joystick_axis_count", ctoy_joystick_axis_count);
   tcc_add_symbol(ctoy__tcc, "ctoy_joystick_button_count", ctoy_joystick_button_count);
   tcc_add_symbol(ctoy__tcc, "ctoy_joystick_button_press", ctoy_joystick_button_press);
   tcc_add_symbol(ctoy__tcc, "ctoy_joystick_button_release", ctoy_joystick_button_release);
   tcc_add_symbol(ctoy__tcc, "ctoy_joystick_button_pressed", ctoy_joystick_button_pressed);
   tcc_add_symbol(ctoy__tcc, "ctoy_joystick_axis", ctoy_joystick_axis);

   tcc_add_symbol(ctoy__tcc, "ctoy_get_chars", ctoy_get_chars);
   tcc_add_symbol(ctoy__tcc, "ctoy_window_size", ctoy_window_size);
   tcc_add_symbol(ctoy__tcc, "ctoy_render_image", ctoy_render_image);
   tcc_add_symbol(ctoy__tcc, "ctoy_swap_buffer", ctoy_swap_buffer);
   tcc_add_symbol(ctoy__tcc, "ctoy_window_fullscreen", ctoy_window_fullscreen);
   tcc_add_symbol(ctoy__tcc, "ctoy_window_title", ctoy_window_title);
   tcc_add_symbol(ctoy__tcc, "ctoy_register_memory", ctoy_register_memory);
   tcc_add_symbol(ctoy__tcc, "ctoy_retrieve_memory", ctoy_retrieve_memory);

   tcc_add_symbol(ctoy__tcc, "ctoy_argc", ctoy_argc);
   tcc_add_symbol(ctoy__tcc, "ctoy_argv", ctoy_argv);
}

void ctoy__maratis_symbols(void)
{
   tcc_add_symbol(ctoy__tcc, "m_srand", m_srand);
   tcc_add_symbol(ctoy__tcc, "m_rand", m_rand);
   tcc_add_symbol(ctoy__tcc, "m_randf", m_randf);
   tcc_add_symbol(ctoy__tcc, "m_rand_user", m_rand_user);
   tcc_add_symbol(ctoy__tcc, "m_randf_user", m_randf_user);
   tcc_add_symbol(ctoy__tcc, "m_normalize", m_normalize);
   tcc_add_symbol(ctoy__tcc, "m_normalize_sum", m_normalize_sum);
   tcc_add_symbol(ctoy__tcc, "m_mean", m_mean);
   tcc_add_symbol(ctoy__tcc, "m_convolution", m_convolution);
   tcc_add_symbol(ctoy__tcc, "m_squared_distance", m_squared_distance);
   tcc_add_symbol(ctoy__tcc, "m_chi_squared_distance", m_chi_squared_distance);
   tcc_add_symbol(ctoy__tcc, "m_gaussian_kernel", m_gaussian_kernel);
   tcc_add_symbol(ctoy__tcc, "m_sst", m_sst);
   tcc_add_symbol(ctoy__tcc, "m_harris_response", m_harris_response);
   tcc_add_symbol(ctoy__tcc, "m_tfm", m_tfm);
   tcc_add_symbol(ctoy__tcc, "m_interpolation_cubic", m_interpolation_cubic);
   tcc_add_symbol(ctoy__tcc, "m_interpolation_catmullrom", m_interpolation_catmullrom);
   tcc_add_symbol(ctoy__tcc, "m_2d_line_to_line_intersection", m_2d_line_to_line_intersection);
   tcc_add_symbol(ctoy__tcc, "m_2d_box_to_box_collision", m_2d_box_to_box_collision);
   tcc_add_symbol(ctoy__tcc, "m_2d_polygon_area", m_2d_polygon_area);
   tcc_add_symbol(ctoy__tcc, "m_2d_polyline_length", m_2d_polyline_length);
   tcc_add_symbol(ctoy__tcc, "m_3d_box_to_box_collision", m_3d_box_to_box_collision);
   tcc_add_symbol(ctoy__tcc, "m_3d_ray_box_intersection_in_out", m_3d_ray_box_intersection_in_out);
   tcc_add_symbol(ctoy__tcc, "m_3d_ray_sphere_intersection_in_out", m_3d_ray_sphere_intersection_in_out);
   tcc_add_symbol(ctoy__tcc, "m_3d_tri_box_overlap", m_3d_tri_box_overlap);
   tcc_add_symbol(ctoy__tcc, "m_3d_polyline_length", m_3d_polyline_length);
   tcc_add_symbol(ctoy__tcc, "m_3d_ray_plane_intersection", m_3d_ray_plane_intersection);
   tcc_add_symbol(ctoy__tcc, "m_3d_ray_sphere_intersection", m_3d_ray_sphere_intersection);
   tcc_add_symbol(ctoy__tcc, "m_3d_ray_box_intersection", m_3d_ray_box_intersection);
   tcc_add_symbol(ctoy__tcc, "m_3d_ray_triangle_intersection", m_3d_ray_triangle_intersection);
   
   tcc_add_symbol(ctoy__tcc, "m_quat_identity", m_quat_identity);
   tcc_add_symbol(ctoy__tcc, "m_quat_normalize", m_quat_normalize);
   tcc_add_symbol(ctoy__tcc, "m_quat_rotation_axis", m_quat_rotation_axis);
   tcc_add_symbol(ctoy__tcc, "m_quat_rotation_euler", m_quat_rotation_euler);
   tcc_add_symbol(ctoy__tcc, "m_quat_mul", m_quat_mul);
   tcc_add_symbol(ctoy__tcc, "m_quat_slerp", m_quat_slerp);
   tcc_add_symbol(ctoy__tcc, "m_quat_extract_axis_angle", m_quat_extract_axis_angle);

   tcc_add_symbol(ctoy__tcc, "m_mat4_identity", m_mat4_identity);
   tcc_add_symbol(ctoy__tcc, "m_mat4_perspective", m_mat4_perspective);
   tcc_add_symbol(ctoy__tcc, "m_mat4_ortho", m_mat4_ortho);
   tcc_add_symbol(ctoy__tcc, "m_mat4_lookat", m_mat4_lookat);
   tcc_add_symbol(ctoy__tcc, "m_mat4_translation", m_mat4_translation);
   tcc_add_symbol(ctoy__tcc, "m_mat4_rotation_axis", m_mat4_rotation_axis);
   tcc_add_symbol(ctoy__tcc, "m_mat4_rotation_euler", m_mat4_rotation_euler);
   tcc_add_symbol(ctoy__tcc, "m_mat4_scale", m_mat4_scale);
   tcc_add_symbol(ctoy__tcc, "m_mat4_mul", m_mat4_mul);
   tcc_add_symbol(ctoy__tcc, "m_mat4_transpose", m_mat4_transpose);
   tcc_add_symbol(ctoy__tcc, "m_mat4_inverse_transpose", m_mat4_inverse_transpose);
   tcc_add_symbol(ctoy__tcc, "m_mat4_inverse", m_mat4_inverse);
   tcc_add_symbol(ctoy__tcc, "m_mat4_extract_euler", m_mat4_extract_euler);
   tcc_add_symbol(ctoy__tcc, "m_mat4_rotate3", m_mat4_rotate3);
   tcc_add_symbol(ctoy__tcc, "m_mat4_inverse_rotate3", m_mat4_inverse_rotate3);
   tcc_add_symbol(ctoy__tcc, "m_mat4_transform3", m_mat4_transform3);
   tcc_add_symbol(ctoy__tcc, "m_mat4_transform4", m_mat4_transform4);

   tcc_add_symbol(ctoy__tcc, "m_type_sizeof", m_type_sizeof);
   tcc_add_symbol(ctoy__tcc, "m_image_create", m_image_create);
   tcc_add_symbol(ctoy__tcc, "m_image_destroy", m_image_destroy);
   tcc_add_symbol(ctoy__tcc, "m_image_ubyte_to_float", m_image_ubyte_to_float);
   tcc_add_symbol(ctoy__tcc, "m_image_ushort_to_float", m_image_ushort_to_float);
   tcc_add_symbol(ctoy__tcc, "m_image_half_to_float", m_image_half_to_float);
   tcc_add_symbol(ctoy__tcc, "m_image_float_to_ubyte", m_image_float_to_ubyte);
   tcc_add_symbol(ctoy__tcc, "m_image_float_to_srgb", m_image_float_to_srgb);
   tcc_add_symbol(ctoy__tcc, "m_image_float_to_ushort", m_image_float_to_ushort);
   tcc_add_symbol(ctoy__tcc, "m_image_float_to_half", m_image_float_to_half);
   tcc_add_symbol(ctoy__tcc, "m_image_copy", m_image_copy);
   tcc_add_symbol(ctoy__tcc, "m_image_copy_sub_image", m_image_copy_sub_image);
   tcc_add_symbol(ctoy__tcc, "m_image_reframe", m_image_reframe);
   tcc_add_symbol(ctoy__tcc, "m_image_reframe_zero", m_image_reframe_zero);
   tcc_add_symbol(ctoy__tcc, "m_image_extract_component", m_image_extract_component);
   tcc_add_symbol(ctoy__tcc, "m_image_rotate_left", m_image_rotate_left);
   tcc_add_symbol(ctoy__tcc, "m_image_rotate_right", m_image_rotate_right);
   tcc_add_symbol(ctoy__tcc, "m_image_rotate_180", m_image_rotate_180);
   tcc_add_symbol(ctoy__tcc, "m_image_mirror_x", m_image_mirror_x);
   tcc_add_symbol(ctoy__tcc, "m_image_mirror_y", m_image_mirror_y);
   
   tcc_add_symbol(ctoy__tcc, "m_image_premultiply", m_image_premultiply);
   tcc_add_symbol(ctoy__tcc, "m_image_unpremultiply", m_image_unpremultiply);
   tcc_add_symbol(ctoy__tcc, "m_image_sRGB_to_linear", m_image_sRGB_to_linear);
   tcc_add_symbol(ctoy__tcc, "m_image_linear_to_sRGB", m_image_linear_to_sRGB);

   tcc_add_symbol(ctoy__tcc, "m_image_sub_pixel", m_image_sub_pixel);
   
   tcc_add_symbol(ctoy__tcc, "m_image_summed_area", m_image_summed_area);
   tcc_add_symbol(ctoy__tcc, "m_image_convolution_h_raw", m_image_convolution_h_raw);
   tcc_add_symbol(ctoy__tcc, "m_image_convolution_v_raw", m_image_convolution_v_raw);   
   tcc_add_symbol(ctoy__tcc, "m_image_convolution_h", m_image_convolution_h);
   tcc_add_symbol(ctoy__tcc, "m_image_convolution_v", m_image_convolution_v);
   tcc_add_symbol(ctoy__tcc, "m_image_gaussian_blur", m_image_gaussian_blur);
   tcc_add_symbol(ctoy__tcc, "m_image_grey", m_image_grey);
   tcc_add_symbol(ctoy__tcc, "m_image_max", m_image_max);
   tcc_add_symbol(ctoy__tcc, "m_image_max_abs", m_image_max_abs);
   tcc_add_symbol(ctoy__tcc, "m_image_sobel", m_image_sobel);
   tcc_add_symbol(ctoy__tcc, "m_image_harris", m_image_harris);
   
   tcc_add_symbol(ctoy__tcc, "m_image_pyrdown", m_image_pyrdown);
   tcc_add_symbol(ctoy__tcc, "m_image_resize", m_image_resize);
   
   tcc_add_symbol(ctoy__tcc, "m_image_non_max_supp", m_image_non_max_supp);
   tcc_add_symbol(ctoy__tcc, "m_image_corner_harris", m_image_corner_harris);
   
   tcc_add_symbol(ctoy__tcc, "m_image_floodfill_4x", m_image_floodfill_4x);
   tcc_add_symbol(ctoy__tcc, "m_image_floodfill_8x", m_image_floodfill_8x);
   tcc_add_symbol(ctoy__tcc, "m_image_dilate", m_image_dilate);
   tcc_add_symbol(ctoy__tcc, "m_image_erode", m_image_erode);
   tcc_add_symbol(ctoy__tcc, "m_image_edge_4x", m_image_edge_4x);
   tcc_add_symbol(ctoy__tcc, "m_image_thin", m_image_thin);

   tcc_add_symbol(ctoy__tcc, "m_raster_inv_bilerp", m_raster_inv_bilerp);
   tcc_add_symbol(ctoy__tcc, "m_raster_triangle_bbox_att4", m_raster_triangle_bbox_att4);
   tcc_add_symbol(ctoy__tcc, "m_raster_triangle_att4", m_raster_triangle_att4);
   tcc_add_symbol(ctoy__tcc, "m_raster_line", m_raster_line);
   tcc_add_symbol(ctoy__tcc, "m_raster_circle", m_raster_circle);
   tcc_add_symbol(ctoy__tcc, "m_raster_polygon", m_raster_polygon);
   
   tcc_add_symbol(ctoy__tcc, "m_dist_transform_1d", m_dist_transform_1d);
   tcc_add_symbol(ctoy__tcc, "m_dist_transform_2d", m_dist_transform_2d);
   tcc_add_symbol(ctoy__tcc, "m_voronoi_transform_1d", m_voronoi_transform_1d);
   tcc_add_symbol(ctoy__tcc, "m_voronoi_transform_2d", m_voronoi_transform_2d);
   tcc_add_symbol(ctoy__tcc, "m_image_dist_mask_init", m_image_dist_mask_init);
   tcc_add_symbol(ctoy__tcc, "m_image_dist_transform", m_image_dist_transform);
   tcc_add_symbol(ctoy__tcc, "m_image_voronoi_transform", m_image_voronoi_transform);
   tcc_add_symbol(ctoy__tcc, "m_image_voronoi_fill", m_image_voronoi_fill);
   
   tcc_add_symbol(ctoy__tcc, "m_sRGB_to_linear", m_sRGB_to_linear);
   tcc_add_symbol(ctoy__tcc, "m_linear_to_sRGB", m_linear_to_sRGB);
   tcc_add_symbol(ctoy__tcc, "m_RGB_to_HSV", m_RGB_to_HSV);
   tcc_add_symbol(ctoy__tcc, "m_HSV_to_RGB", m_HSV_to_RGB);
   tcc_add_symbol(ctoy__tcc, "m_RGB_to_HSL", m_RGB_to_HSL);
   tcc_add_symbol(ctoy__tcc, "m_HSL_to_RGB", m_HSL_to_RGB);

   tcc_add_symbol(ctoy__tcc, "m_pf_floodfill", m_pf_floodfill);
   tcc_add_symbol(ctoy__tcc, "m_pf_backtrace", m_pf_backtrace);
}

void ctoy__tcc_symbols(void)
{
   tcc_add_symbol(ctoy__tcc, "tcc_new", tcc_new);
   tcc_add_symbol(ctoy__tcc, "tcc_delete", tcc_delete);
   tcc_add_symbol(ctoy__tcc, "tcc_set_lib_path", tcc_set_lib_path);
   tcc_add_symbol(ctoy__tcc, "tcc_set_error_func", tcc_set_error_func);
   tcc_add_symbol(ctoy__tcc, "tcc_set_options", tcc_set_options); 
   tcc_add_symbol(ctoy__tcc, "tcc_add_include_path", tcc_add_include_path);
   tcc_add_symbol(ctoy__tcc, "tcc_add_sysinclude_path", tcc_add_sysinclude_path);
   tcc_add_symbol(ctoy__tcc, "tcc_define_symbol", tcc_define_symbol);
   tcc_add_symbol(ctoy__tcc, "tcc_undefine_symbol", tcc_undefine_symbol);
   tcc_add_symbol(ctoy__tcc, "tcc_add_file", tcc_add_file);
   tcc_add_symbol(ctoy__tcc, "tcc_compile_string", tcc_compile_string);
   tcc_add_symbol(ctoy__tcc, "tcc_set_output_type", tcc_set_output_type);
   tcc_add_symbol(ctoy__tcc, "tcc_add_library_path", tcc_add_library_path);
   tcc_add_symbol(ctoy__tcc, "tcc_add_library", tcc_add_library);
   tcc_add_symbol(ctoy__tcc, "tcc_add_symbol", tcc_add_symbol);
   tcc_add_symbol(ctoy__tcc, "tcc_output_file", tcc_output_file);
   tcc_add_symbol(ctoy__tcc, "tcc_run", tcc_run);
   tcc_add_symbol(ctoy__tcc, "tcc_relocate", tcc_relocate);
   tcc_add_symbol(ctoy__tcc, "tcc_get_symbol", tcc_get_symbol);
}

void ctoy__glfw_symbols(void)
{
   tcc_add_symbol(ctoy__tcc, "ctoy__get_glfw_window", ctoy__get_glfw_window);
   tcc_add_symbol(ctoy__tcc, "glfwGetClipboardString", glfwGetClipboardString);
   tcc_add_symbol(ctoy__tcc, "glfwSetClipboardString", glfwSetClipboardString);
   tcc_add_symbol(ctoy__tcc, "glfwDestroyCursor", glfwDestroyCursor);
   tcc_add_symbol(ctoy__tcc, "glfwSetCursorPos", glfwSetCursorPos);
   tcc_add_symbol(ctoy__tcc, "glfwGetCursorPos", glfwGetCursorPos);
   tcc_add_symbol(ctoy__tcc, "glfwGetWindowAttrib", glfwGetWindowAttrib);
   tcc_add_symbol(ctoy__tcc, "glfwGetInputMode", glfwGetInputMode);
   tcc_add_symbol(ctoy__tcc, "glfwSetInputMode", glfwSetInputMode);
   tcc_add_symbol(ctoy__tcc, "glfwCreateStandardCursor", glfwCreateStandardCursor);
   tcc_add_symbol(ctoy__tcc, "glfwSetCursor", glfwSetCursor);
}

void ctoy__all_symbols(void)
{
   ctoy__system_symbols();
   ctoy__gles2_symbols();
   ctoy__symbols();
   ctoy__maratis_symbols();
   ctoy__tcc_symbols();
   ctoy__glfw_symbols();
}

void ctoy__add_libs(void)
{
   char filename[256];
   DIR *pdir = opendir("lib");
   if (pdir) {

      struct dirent *pent = NULL;
      while ((pent = readdir(pdir))) {
         
         if (pent->d_name[0] == '.')
            continue;

         sprintf(filename, "lib/%s", pent->d_name);
         if (! ctoy__is_directory(filename)) {
#ifdef WIN32
            LoadLibrary(filename);
            tcc_add_file(ctoy__tcc, filename, TCC_FILETYPE_BINARY);
#else
            dlopen(filename, RTLD_LAZY);
	#ifndef __APPLE__
            tcc_add_file(ctoy__tcc, filename, TCC_FILETYPE_BINARY);
	#endif
#endif
         }
      }

      closedir(pdir);
   }
}

int ctoy__tcc_init(void)
{
   char path[256];

#ifndef CTOY_PLAYER
   ctoy__src_count = 0;
#endif
   ctoy__tcc = tcc_new();
   if (!ctoy__tcc) {
      fprintf(stderr, "ERROR TCC: could not create tcc state\n");
      return 0;
   }
   
#ifndef CTOY_PLAYER
   ctoy__io_replace();
#endif

   /* search path */
   tcc_set_lib_path(ctoy__tcc, ".");
   tcc_add_library_path(ctoy__tcc, ctoy__dir);

   sprintf(path, "%s/include", ctoy__dir);
   tcc_add_include_path(ctoy__tcc, path);
   
   sprintf(path, "%s/include/libc", ctoy__dir);
   tcc_add_include_path(ctoy__tcc, path);

   sprintf(path, "%s/include/libc/winapi", ctoy__dir);
   tcc_add_include_path(ctoy__tcc, path);
   
   /* compilation */
   tcc_set_output_type(ctoy__tcc, TCC_OUTPUT_MEMORY);
   
#ifdef WIN32
   tcc_add_library(ctoy__tcc, "msvcrt");
   tcc_add_library(ctoy__tcc, "opengl32");
   tcc_add_library(ctoy__tcc, "OpenAL32");
#endif
   ctoy__add_libs();

   /* main.c */
   if (tcc_add_file(ctoy__tcc, "src/main.c", TCC_FILETYPE_C) == -1)
        return 0;

   /* symbols */
   ctoy__all_symbols();

   /* relocate the code */
   if (tcc_relocate(ctoy__tcc, TCC_RELOCATE_AUTO) < 0)
      return 0;

    /* get entry symbol */
   ctoy_begin = (void (*)(void))tcc_get_symbol(ctoy__tcc, "ctoy_begin");
   ctoy_main_loop = (void (*)(void))tcc_get_symbol(ctoy__tcc, "ctoy_main_loop");
   ctoy_end = (void (*)(void))tcc_get_symbol(ctoy__tcc, "ctoy_end");
   if (!ctoy_begin || !ctoy_main_loop || !ctoy_end)
      return 0;

   return 1;
}

void ctoy__main_loop(void)
{
   if (ctoy__src_state) ctoy_main_loop();
   ctoy__update();
   
   if (ctoy__src_update) {
      if (ctoy__src_state)
         ctoy_end();
      tcc_delete(ctoy__tcc);
      ctoy__src_state = ctoy__tcc_init();
      if (ctoy__src_state)
         ctoy_begin();
#ifndef CTOY_PLAYER
      ctoy__src_thread_restart();
#endif
   }
}

int main(int argc, char **argv)
{
   int i, sz;
   
   ctoy__argc = argc;
   ctoy__argv = argv;

#ifdef __linux__
   {
   	char tmp[256];
      readlink( "/proc/self/exe", tmp, 256);
      ctoy__get_directory(ctoy__dir, tmp);
   }
#else
   ctoy__get_directory(ctoy__dir, argv[0]);
#endif
   ctoy__set_working_dir(ctoy__dir);

   /* openal */
   ctoy__oal_init();

   /* window */
   if (! ctoy__create("CTOY", 512, 512)) {
      printf("ERROR CTOY: could not create window\n");
      ctoy__oal_destroy();
      return EXIT_FAILURE;
   }

   ctoy__src_state = ctoy__tcc_init();

#ifndef CTOY_PLAYER
   ctoy__src_thread_restart();
#endif

   /* run */
   if (ctoy__src_state)
      ctoy_begin();

   while (ctoy__state) {
      ctoy__main_loop();
      thrd_yield();
   }

   if (ctoy__src_state)
      ctoy_end();

   /* destroy */
#ifndef CTOY_PLAYER
   ctoy__src_thread_destroy();
#endif
   tcc_delete(ctoy__tcc);
   ctoy__destroy();
   ctoy__oal_destroy();
   return EXIT_SUCCESS;
}
