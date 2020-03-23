/*======================================================================
 CTOY
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

#include "ctoy.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(_MSC_VER) && __STDC_VERSION__ < 199901L
#define inline __forceinline
#endif

#ifdef WIN32
#include <windows.h>
#include <WinBase.h>
#include <direct.h>
#define mkdir(f, o) _mkdir(f)
#else
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <GLES2/gl2.h>
#define GLFW_INCLUDE_ES2
#else
#include <glad/glad.h>
#include <tinycthread.h>
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>

#define M_MATH_IMPLEMENTATION
#define M_IMAGE_IMPLEMENTATION
#define M_DIST_IMPLEMENTATION
#define M_RASTER_IMPLEMENTATION
#define M_PF_IMPLEMENTATION
#include "m_math.h"
#include "m_image.h"
#include "m_dist.h"
#include "m_raster.h"
#include "m_path_finding.h"
#undef M_MATH_IMPLEMENTATION
#undef M_IMAGE_IMPLEMENTATION
#undef M_DIST_IMPLEMENTATION
#undef M_RASTER_IMPLEMENTATION
#undef M_PF_IMPLEMENTATION

/* system */
char           ctoy__title[256];
struct m_image ctoy__buffer_ubyte = M_IMAGE_IDENTITY();
unsigned long  ctoy__t = 0;
void *         ctoy__memory = NULL;
GLFWwindow *   ctoy__window = NULL;
GLuint         ctoy__texture;
int            ctoy__state = 1;
int            ctoy__prev_win_x = 0;
int            ctoy__prev_win_y = 0;
int            ctoy__prev_win_width = 0;
int            ctoy__prev_win_height = 0;
int            ctoy__win_width = 0;
int            ctoy__win_height = 0;
int            ctoy__fb_width = 0;
int            ctoy__fb_height = 0;
int            ctoy__tex_width = 0;
int            ctoy__tex_height = 0;
char           **ctoy__argv;
int            ctoy__argc = 0;


/* input */
#define              CTOY_MOUSE_BUTTON_COUNT (GLFW_MOUSE_BUTTON_LAST+1)
#define              CTOY_KEY_COUNT (GLFW_KEY_LAST+1)
#define              CTOY_JOY_COUNT (GLFW_JOYSTICK_LAST+1)
#define              CTOY_JOY_AXIS_MAX 32
#define              CTOY_JOY_BUTTON_MAX 32
char                 ctoy__button[CTOY_KEY_COUNT][2];
char                 ctoy__mouse_button[CTOY_MOUSE_BUTTON_COUNT][2];
char                 ctoy__joystick_button[CTOY_JOY_COUNT][CTOY_JOY_BUTTON_MAX][2];
float                ctoy__joystick_axis[CTOY_JOY_COUNT][CTOY_JOY_AXIS_MAX];
char                 ctoy__joystick_button_count[CTOY_JOY_COUNT];
char                 ctoy__joystick_axis_count[CTOY_JOY_COUNT];
unsigned int         ctoy__char_queue[CTOY_CHAR_MAX];
struct ctoy_pen_data ctoy__pen_data_queue[CTOY_PEN_DATA_MAX];
int                  ctoy__char_count = 0;
int                  ctoy__pen_data_count = 0;
float                ctoy__mouse_x = 0;
float                ctoy__mouse_y = 0;
float                ctoy__scroll_x = 0;
float                ctoy__scroll_y = 0;

/* sound */
ALCdevice *    ctoy__oal_device = NULL;
ALCcontext *   ctoy__oal_context = NULL;

/* shader */
static char ctoy__vert_src[] =
"attribute vec2 aVertex;\n"
"varying vec2 vTexcoord;"
"void main()"
"{"
" vec2 p = aVertex.xy - 0.5;"
" vTexcoord = vec2(aVertex.x, 1.0-aVertex.y);"
" gl_Position = vec4(p.x*2.0, p.y*2.0, 0.0, 1.0); \n"
"}";                             

static char ctoy__frag_src[] =
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"uniform sampler2D uTexture0;"
"varying vec2 vTexcoord;"
"void main()"
"{"
" gl_FragColor = texture2D(uTexture0, vTexcoord);"
"}";

static GLuint ctoy__vert_shader;
static GLuint ctoy__frag_shader;
static GLuint ctoy__prog_object;


GLFWwindow * ctoy__get_glfw_window(void)
{
   return ctoy__window;
}

static void ctoy__close_callback(GLFWwindow * window)
{
   ctoy__state = 0;
}

static void ctoy__size_callback(GLFWwindow * window, int width, int height)
{
   ctoy__win_width = width;
   ctoy__win_height = height;
}

static void ctoy__cursorpos_callback(GLFWwindow * window, double x, double y)
{
   ctoy__mouse_x = (float)x / (float)ctoy__win_width;
   ctoy__mouse_y = (float)y / (float)ctoy__win_height;
}

void ctoy__scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
   ctoy__scroll_x += (float)xoffset / (float)ctoy__win_width;
   ctoy__scroll_y += (float)yoffset / (float)ctoy__win_height;
}

static void ctoy__key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (key >=0 && key < CTOY_KEY_COUNT) {
      ctoy__button[key][0] = action + 1;
      ctoy__button[key][1] = action;
   }
}

static void ctoy__mousebutton_callback(GLFWwindow * window, int button, int action, int mods)
{
   if (button >=0 && button < CTOY_MOUSE_BUTTON_COUNT) {
      ctoy__mouse_button[button][0] = action + 1;
      ctoy__mouse_button[button][1] = action;
   }
}

static void ctoy__char_callback(GLFWwindow * window, unsigned int key)
{
   if (ctoy__char_count < CTOY_CHAR_MAX) {
      ctoy__char_queue[ctoy__char_count] = key;
      ctoy__char_count++;
   }
}

static void ctoy__penTabletData_callback(double x, double y, double z, double pressure, double pitch, double yaw, double roll)
{
   int xpos, ypos;
   glfwGetWindowPos(ctoy__window, &xpos, &ypos);

   if (ctoy__pen_data_count < CTOY_PEN_DATA_MAX) {
      struct ctoy_pen_data pd = {
         (x - xpos) / ctoy__win_width,
         (y - ypos) / ctoy__win_height,
         z, pressure, pitch, yaw, roll};
      ctoy__pen_data_queue[ctoy__pen_data_count] = pd;
      ctoy__pen_data_count++;
   }
   else {
      assert(0);
   }
}

static void ctoy__penTabletCursor_callback(unsigned int cursor)
{
}

static void ctoy__penTabletProximity_callback(int proximity)
{
}

static GLuint ctoy__shader(GLenum type, const char *src)
{
    GLuint shader; GLint compiled;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static void ctoy__draw_texture(GLuint texture)
{
  float vertices[8] = {0, 0, 0, 1, 1, 0, 1, 1};

  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUseProgram(ctoy__prog_object);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertices);
  glEnableVertexAttribArray(0);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glUseProgram(0);
}

static void ctoy__setup_texture(int width, int height)
{
  glBindTexture(GL_TEXTURE_2D, ctoy__texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  ctoy__tex_width = width;
  ctoy__tex_height = height;
}

static GLFWmonitor *ctoy__monitor_init(void)
{
   GLFWmonitor *monitor = glfwGetPrimaryMonitor();
   const GLFWvidmode *mode = glfwGetVideoMode(monitor);
   
   glfwWindowHint(GLFW_RED_BITS, mode->redBits);
   glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
   glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
   glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
   
   ctoy__win_width = mode->width;
   ctoy__win_height = mode->height;
   return monitor;
}

static int ctoy__window_init(const char *title, int fullscreen)
{
   GLFWmonitor *monitor = NULL;
   GLFWwindow *win;

   if (fullscreen)
      monitor = ctoy__monitor_init();
   
#ifdef GLFW_INCLUDE_ES2
   glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#endif

   win = glfwCreateWindow(ctoy__win_width, ctoy__win_height, title, monitor, ctoy__window);
   if (win == NULL)
      return 0;
   
   if (ctoy__window)
      glfwDestroyWindow(ctoy__window);

   ctoy__window = win;
   sprintf(ctoy__title, "%s", title);
   
   glfwSetWindowCloseCallback(ctoy__window, ctoy__close_callback);
   glfwSetWindowSizeCallback(ctoy__window, ctoy__size_callback);
   glfwSetKeyCallback(ctoy__window, ctoy__key_callback);
   glfwSetMouseButtonCallback(ctoy__window, ctoy__mousebutton_callback);
   glfwSetCharCallback(ctoy__window, ctoy__char_callback);
   glfwSetCursorPosCallback(ctoy__window, ctoy__cursorpos_callback);
   glfwSetScrollCallback(ctoy__window, ctoy__scroll_callback);
   glfwSetPenTabletDataCallback(ctoy__penTabletData_callback);
   glfwSetPenTabletCursorCallback(ctoy__penTabletCursor_callback);
   glfwSetPenTabletProximityCallback(ctoy__penTabletProximity_callback);
   glfwMakeContextCurrent(ctoy__window);
   glfwSwapInterval(1);

#ifndef GLFW_INCLUDE_ES2
   if (gladLoadGL() == 0)
      return 0;
#endif

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glPixelStorei(GL_PACK_ALIGNMENT, 1);
   glEnable(GL_TEXTURE_2D);

   glfwGetFramebufferSize(ctoy__window, &ctoy__fb_width, &ctoy__fb_height);
   
   return 1;
}

static void ctoy__joy_update(int joy)
{
   const float *axis;
   const unsigned char *button;
   int axis_count, button_count;

   axis = glfwGetJoystickAxes(joy, &axis_count);
   button = glfwGetJoystickButtons(joy, &button_count);

   if (axis) {
      int c = M_MIN(axis_count, CTOY_JOY_AXIS_MAX);
      ctoy__joystick_axis_count[joy] = c;
      memcpy(ctoy__joystick_axis[joy], axis, c * sizeof(float));
   }

   if (button) {
      int i, c = M_MIN(button_count, CTOY_JOY_BUTTON_MAX);
      ctoy__joystick_button_count[joy] = c;
      for (i = 0; i < c; i++) {
         int s0 = ctoy__joystick_button[joy][i][1];
         int s1 = button[i];
         ctoy__joystick_button[joy][i][0] = s1 - s0;
         ctoy__joystick_button[joy][i][1] = s1;
      }
   }
}

static int ctoy__create(const char *title, int width, int height)
{
   int i;

   memset(ctoy__button, 0, sizeof(ctoy__button));
   memset(ctoy__mouse_button, 0, sizeof(ctoy__mouse_button));
   memset(ctoy__joystick_button, 0, sizeof(ctoy__joystick_button));
   memset(ctoy__joystick_axis, 0, sizeof(ctoy__joystick_axis));
   memset(ctoy__joystick_button_count, 0, sizeof(ctoy__joystick_button_count));
   memset(ctoy__joystick_axis_count, 0, sizeof(ctoy__joystick_axis_count));

   glfwInit();

   ctoy__win_width = width;
   ctoy__win_height = height;
   ctoy__tex_width = width;
   ctoy__tex_height = height;

   if (! ctoy__window_init(title, 0))
      return 0;

   /* joysticks */
   for (i = 0; i < CTOY_JOY_COUNT; i++)
      ctoy__joy_update(i);

   /* texture */
   glGenTextures(1, &ctoy__texture);
   ctoy__setup_texture(ctoy__tex_width, ctoy__tex_height);

   /* shader */
   ctoy__vert_shader = ctoy__shader(GL_VERTEX_SHADER, ctoy__vert_src);
   ctoy__frag_shader = ctoy__shader(GL_FRAGMENT_SHADER, ctoy__frag_src);
   ctoy__prog_object = glCreateProgram();

   glAttachShader(ctoy__prog_object, ctoy__vert_shader);
   glAttachShader(ctoy__prog_object, ctoy__frag_shader);
   glBindAttribLocation(ctoy__prog_object, 0, "aVertex");
   glLinkProgram(ctoy__prog_object);

   glUseProgram(ctoy__prog_object);
   glUniform1i(glGetUniformLocation(ctoy__prog_object, "uTexture0"), 0);

   return 1;
}

static void ctoy__get_directory(char *dest, const char *src)
{
   char *s;
   strcpy(dest, src);
#ifdef WIN32
   s = strrchr(dest,'\\');
#else
   s = strrchr(dest,'/');
#endif
   if (s) *s = '\0';
}

static void ctoy__set_working_dir(const char *dir)
{
#ifdef WIN32
   SetCurrentDirectory(dir);
#else
   chdir(dir);
#endif
}

static void ctoy__destroy(void)
{
   glDeleteProgram(ctoy__prog_object);
   glDeleteShader(ctoy__frag_shader);
   glDeleteShader(ctoy__vert_shader);
   glDeleteTextures(1, &ctoy__texture);
   glfwMakeContextCurrent(NULL);
   glfwTerminate();
   ctoy__window = NULL;
   m_image_destroy(&ctoy__buffer_ubyte);
}

static void ctoy__update(void)
{
   int i;

   /* joystick */
   for (i = 0; i < CTOY_JOY_COUNT; i++)
      ctoy__joy_update(i);

   /* flush events */
   for (i = 0; i < CTOY_KEY_COUNT; i++)
      ctoy__button[i][0] = 0;
   for (i = 0; i < CTOY_MOUSE_BUTTON_COUNT; i++)
      ctoy__mouse_button[i][0] = 0; 
   ctoy__char_count = 0;
   ctoy__pen_data_count = 0;

   ctoy__scroll_x = 0;
   ctoy__scroll_y = 0;

   glfwPollEvents();
   
   glfwGetFramebufferSize(ctoy__window, &ctoy__fb_width, &ctoy__fb_height);

   ctoy__t++;
}

static int ctoy__oal_init(void)
{
   ctoy__oal_device = alcOpenDevice(NULL);
   if(! ctoy__oal_device)
   {
      printf("ERROR OpenAL: unable to create device\n");
        return 0;
   }

    // context
   ctoy__oal_context = alcCreateContext(ctoy__oal_device, NULL);
    if(! ctoy__oal_context)
   {
      printf("ERROR OpenAL: unable to create context\n");
        return 0;
   }

   if(! alcMakeContextCurrent(ctoy__oal_context))
   {
      printf("ERROR OpenAL: unable to make current context\n");
        return 0;
   }
  
   return 1;
}

static void ctoy__oal_destroy(void)
{
   alcMakeContextCurrent(NULL);
   alcDestroyContext(ctoy__oal_context);
   alcCloseDevice(ctoy__oal_device);
}


int ctoy_get_chars(unsigned int dest[CTOY_CHAR_MAX])
{
   if (ctoy__char_count > 0)
      memcpy(dest, ctoy__char_queue, ctoy__char_count * sizeof(int));
   return ctoy__char_count;
}

int ctoy_get_pen_data(struct ctoy_pen_data dest[CTOY_PEN_DATA_MAX])
{
   if (ctoy__pen_data_count > 0)
      memcpy(dest, ctoy__pen_data_queue, ctoy__pen_data_count * sizeof(struct ctoy_pen_data));
   return ctoy__pen_data_count;
}

int ctoy_key_press(int key)
{
   return (ctoy__button[key][0] == 2);
}

int ctoy_key_release(int key)
{
   return (ctoy__button[key][0] == 1);
}

int ctoy_key_pressed(int key)
{
   return (ctoy__button[key][1] > 0);
}

int ctoy_mouse_button_press(int button)
{
   return (ctoy__mouse_button[button][0] == 2);
}

int ctoy_mouse_button_release(int button)
{
   return (ctoy__mouse_button[button][0] == 1);
}

int ctoy_mouse_button_pressed(int button)
{
   return (ctoy__mouse_button[button][1] > 0);
}

int ctoy_joystick_present(int joy)
{
   return glfwJoystickPresent(joy);
}

int ctoy_joystick_axis_count(int joy)
{
   return ctoy__joystick_axis_count[joy];
}

int ctoy_joystick_button_count(int joy)
{
   return ctoy__joystick_button_count[joy];
}

int ctoy_joystick_button_press(int joy, int button)
{
   return (ctoy__joystick_button[joy][button][0] > 0);
}

int ctoy_joystick_button_release(int joy, int button)
{
   return (ctoy__joystick_button[joy][button][0] < 0);
}

int ctoy_joystick_button_pressed(int joy, int button)
{
   return ctoy__joystick_button[joy][button][1];
}

float ctoy_joystick_axis(int joy, int axis)
{
   return ctoy__joystick_axis[joy][axis];
}

static GLenum comp_to_gl_format(int comp)
{
	switch (comp) {
	default:
	case 1: return GL_LUMINANCE;
	case 3: return GL_RGB;
	case 4: return GL_RGBA;
	}
}

void ctoy_render_image(struct m_image *image)
{
   if (image->width != ctoy__tex_width || image->height != ctoy__tex_height)
      ctoy__setup_texture(image->width, image->height);

   glBindTexture(GL_TEXTURE_2D, ctoy__texture);

   if (image->type == M_FLOAT) {
      m_image_float_to_srgb(&ctoy__buffer_ubyte, image);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ctoy__buffer_ubyte.width, ctoy__buffer_ubyte.height, comp_to_gl_format(image->comp), GL_UNSIGNED_BYTE, ctoy__buffer_ubyte.data);
   }
   else if (image->type == M_UBYTE) {
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->width, image->height, comp_to_gl_format(image->comp), GL_UNSIGNED_BYTE, image->data);
   }
   
   glScissor(0, 0, ctoy__fb_width, ctoy__fb_height);
   glViewport(0, 0, ctoy__fb_width, ctoy__fb_height);
   ctoy__draw_texture(ctoy__texture);
}

void ctoy_swap_buffer(struct m_image *image)
{
   if (image) ctoy_render_image(image);
   glfwSwapBuffers(ctoy__window);
}

double ctoy_get_time(void)
{
   return glfwGetTime();
}

void ctoy_window_title(const char *title)
{
   glfwSetWindowTitle(ctoy__window, title);
   sprintf(ctoy__title, "%s", title);
}

void ctoy_window_size(int width, int height)
{
   glfwSetWindowSize(ctoy__window, width, height);
   ctoy__win_width = width;
   ctoy__win_height = height;
}

void ctoy_window_fullscreen(int fullscreen)
{
   GLFWmonitor *monitor = NULL;
   int x = 0, y = 0;

   if (fullscreen) {
      ctoy__prev_win_width = ctoy__win_width;
      ctoy__prev_win_height = ctoy__win_height;
      glfwGetWindowPos(ctoy__window, &ctoy__prev_win_x, &ctoy__prev_win_y);
      monitor = ctoy__monitor_init();
   }
   else {
      ctoy__win_width = ctoy__prev_win_width;
      ctoy__win_height = ctoy__prev_win_height;
      x = ctoy__prev_win_x;
      y = ctoy__prev_win_y;
   }

   glfwSetWindowMonitor(
      ctoy__window, monitor,
      x, y, ctoy__win_width, ctoy__win_height, GLFW_DONT_CARE
      );
   glfwSwapInterval(1);
}

unsigned long ctoy_t(void)
{
   return ctoy__t;
}

int ctoy_window_width(void)
{
   return ctoy__win_width;
}

int ctoy_window_height(void)
{
   return ctoy__win_height;
}

int ctoy_frame_buffer_width(void)
{
   return ctoy__fb_width;
}

int ctoy_frame_buffer_height(void)
{
   return ctoy__fb_height;
}

int ctoy_width(void)
{
   return ctoy__tex_width;
}

int ctoy_height(void)
{
   return ctoy__tex_height;
}

float ctoy_mouse_x(void)
{
   return ctoy__mouse_x;
}

float ctoy_mouse_y(void)
{
   return ctoy__mouse_y;
}

float ctoy_scroll_x(void)
{
   return ctoy__scroll_x;
}

float ctoy_scroll_y(void)
{
   return ctoy__scroll_y;
}

void ctoy_register_memory(void *memory)
{
   ctoy__memory = memory;
}

void *ctoy_retrieve_memory(void)
{
   return ctoy__memory;
}

void ctoy_sleep(long sec, long nsec)
{
   struct timespec t;
   t.tv_sec = sec;
   t.tv_nsec = nsec;

#ifdef __EMSCRIPTEN__
   nanosleep(&t, NULL);
#else
   thrd_sleep(&t, NULL);
#endif
}

int ctoy_argc(void)
{
    return ctoy__argc;
}

char **ctoy_argv(void)
{
    return ctoy__argv;
}
