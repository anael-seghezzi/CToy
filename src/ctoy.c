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
#endif

#include <GLFW/glfw3.h>

#define M_MATH_IMPLEMENTATION
#define M_IMAGE_IMPLEMENTATION
#define M_RASTER_IMPLEMENTATION
#define M_COLOR_IMPLEMENTATION
#define M_DIST_IMPLEMENTATION
#include "m_math.h"
#include "m_image.h"
#include "m_color.h"
#include "m_dist.h"
#include "m_raster.h"


/* system */
char           _ctoy_title[256];
struct m_image _ctoy_buffer_ubyte = M_IMAGE_IDENTITY();
unsigned long  _ctoy_t = 0;
void *         _ctoy_memory = NULL;
GLFWwindow *   _ctoy_window = NULL;
GLuint         _ctoy_texture;
int            _ctoy_state = 1;
int            _ctoy_win_width = 0;
int            _ctoy_win_height = 0;
int            _ctoy_tex_width = 0;
int            _ctoy_tex_height = 0;

/* input */
#define        CTOY_MOUSE_BUTTON_COUNT (GLFW_MOUSE_BUTTON_LAST+1)
#define        CTOY_KEY_COUNT (GLFW_KEY_LAST+1)
#define        CTOY_JOY_COUNT (GLFW_JOYSTICK_LAST+1)
#define        CTOY_JOY_AXIS_MAX 32
#define        CTOY_JOY_BUTTON_MAX 32
char           _ctoy_button[CTOY_KEY_COUNT][2];
char           _ctoy_mouse_button[CTOY_MOUSE_BUTTON_COUNT][2];
char           _ctoy_joystick_button[CTOY_JOY_COUNT][CTOY_JOY_BUTTON_MAX][2];
float          _ctoy_joystick_axis[CTOY_JOY_COUNT][CTOY_JOY_AXIS_MAX];
char           _ctoy_joystick_button_count[CTOY_JOY_COUNT];
char           _ctoy_joystick_axis_count[CTOY_JOY_COUNT];
unsigned int   _ctoy_char_queue[CTOY_CHAR_MAX];
int            _ctoy_char_count = 0;
float          _ctoy_mouse_x = 0;
float          _ctoy_mouse_y = 0;

/* sound */
ALCdevice *    _ctoy_oal_device = NULL;
ALCcontext *   _ctoy_oal_context = NULL;

/* shader */
static char _ctoy_vert_src[] =
"attribute vec2 aVertex;\n"
"varying vec2 vTexcoord;"
"void main()"
"{"
" vec2 p = aVertex.xy - 0.5;"
" vTexcoord = vec2(aVertex.x, 1.0-aVertex.y);"
" gl_Position = vec4(p.x*2.0, p.y*2.0, 0.0, 1.0); \n"
"}";                             

static char _ctoy_frag_src[] =
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"uniform sampler2D uTexture0;"
"varying vec2 vTexcoord;"
"void main()"
"{"
" gl_FragColor = texture2D(uTexture0, vTexcoord);"
"}";

static GLuint _ctoy_vert_shader;
static GLuint _ctoy_frag_shader;
static GLuint _ctoy_prog_object;


static void _ctoy_close_callback(GLFWwindow * window)
{
   _ctoy_state = 0;
}

static void _ctoy_size_callback(GLFWwindow * window, int width, int height)
{
   _ctoy_win_width = width;
   _ctoy_win_height = height;
}

static void _ctoy_cursorpos_callback(GLFWwindow * window, double x, double y)
{
   _ctoy_mouse_x = (float)x / (float)_ctoy_win_width;
   _ctoy_mouse_y = (float)y / (float)_ctoy_win_height;
}

static void _ctoy_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (key >=0 && key < CTOY_KEY_COUNT) {
      _ctoy_button[key][0] = action + 1;
      _ctoy_button[key][1] = action;
   }
}

static void _ctoy_mousebutton_callback(GLFWwindow * window, int button, int action, int mods)
{
   if (button >=0 && button < CTOY_MOUSE_BUTTON_COUNT) {
      _ctoy_mouse_button[button][0] = action + 1;
      _ctoy_mouse_button[button][1] = action;
   }
}

static void _ctoy_char_callback(GLFWwindow * window, unsigned int key)
{
   if (_ctoy_char_count < CTOY_CHAR_MAX) {
      _ctoy_char_queue[_ctoy_char_count] = key;
      _ctoy_char_count++;
   }
}

static GLuint _ctoy_shader(GLenum type, const char *src)
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

static void _ctoy_draw_texture(GLuint texture)
{
  float vertices[8] = {0, 0, 0, 1, 1, 0, 1, 1};

  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUseProgram(_ctoy_prog_object);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertices);
  glEnableVertexAttribArray(0);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glUseProgram(0);
}

static void _ctoy_setup_texture(int width, int height)
{
  glBindTexture(GL_TEXTURE_2D, _ctoy_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  _ctoy_tex_width = width;
  _ctoy_tex_height = height;
}

static int _ctoy_window_init(const char *title, int fullscreen)
{
   GLFWmonitor *monitor = NULL;
   GLFWwindow *win;

   if (fullscreen) {
      const GLFWvidmode *mode;
      monitor = glfwGetPrimaryMonitor();
      mode = glfwGetVideoMode(monitor);
      
      glfwWindowHint(GLFW_RED_BITS, mode->redBits);
      glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
      glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
      glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
      
      _ctoy_win_width = mode->width;
      _ctoy_win_height = mode->height;
   }
   
#ifdef GLFW_INCLUDE_ES2
   glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#endif

   win = glfwCreateWindow(_ctoy_win_width, _ctoy_win_height, title, monitor, _ctoy_window);
   if (win == NULL)
      return 0;
   
   if (_ctoy_window)
      glfwDestroyWindow(_ctoy_window);
      
   _ctoy_window = win;
   sprintf(_ctoy_title, "%s", title);
   
   glfwSetWindowCloseCallback(_ctoy_window, _ctoy_close_callback);
   glfwSetWindowSizeCallback(_ctoy_window, _ctoy_size_callback);
   glfwSetKeyCallback(_ctoy_window, _ctoy_key_callback);
   glfwSetMouseButtonCallback(_ctoy_window, _ctoy_mousebutton_callback);
   glfwSetCharCallback(_ctoy_window, _ctoy_char_callback);
   glfwSetCursorPosCallback(_ctoy_window, _ctoy_cursorpos_callback);
   glfwMakeContextCurrent(_ctoy_window);
   
#ifndef GLFW_INCLUDE_ES2
   if (gladLoadGL() == 0)
      return 0;
#endif

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glPixelStorei(GL_PACK_ALIGNMENT, 1);
   glEnable(GL_TEXTURE_2D);
   
   return 1;
}

static int _ctoy_create(const char *title, int width, int height)
{
   memset(_ctoy_button, 0, sizeof(_ctoy_button));
   memset(_ctoy_mouse_button, 0, sizeof(_ctoy_mouse_button));
   memset(_ctoy_joystick_button, 0, sizeof(_ctoy_joystick_button));
   memset(_ctoy_joystick_axis, 0, sizeof(_ctoy_joystick_axis));
   memset(_ctoy_joystick_button_count, 0, sizeof(_ctoy_joystick_button_count));
   memset(_ctoy_joystick_axis_count, 0, sizeof(_ctoy_joystick_axis_count));

   glfwInit();

   _ctoy_win_width = width;
   _ctoy_win_height = height;
   _ctoy_tex_width = width;
   _ctoy_tex_height = height;

   if (! _ctoy_window_init(title, 0))
      return 0;

   /* texture */
   glGenTextures(1, &_ctoy_texture);
   _ctoy_setup_texture(_ctoy_tex_width, _ctoy_tex_height);

   /* shader */
   _ctoy_vert_shader = _ctoy_shader(GL_VERTEX_SHADER, _ctoy_vert_src);
   _ctoy_frag_shader = _ctoy_shader(GL_FRAGMENT_SHADER, _ctoy_frag_src);
   _ctoy_prog_object = glCreateProgram();

   glAttachShader(_ctoy_prog_object, _ctoy_vert_shader);
   glAttachShader(_ctoy_prog_object, _ctoy_frag_shader);
   glBindAttribLocation(_ctoy_prog_object, 0, "aVertex");
   glLinkProgram(_ctoy_prog_object);

   glUseProgram(_ctoy_prog_object);
   glUniform1i(glGetUniformLocation(_ctoy_prog_object, "uTexture0"), 0);

   return 1;
}

static void _ctoy_get_directory(char *dest, const char *src)
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

static void _ctoy_set_working_dir(const char *dir)
{
#ifdef WIN32
   SetCurrentDirectory(dir);
#else
   chdir(dir);
#endif
}

static void _ctoy_destroy(void)
{
   glDeleteProgram(_ctoy_prog_object);
   glDeleteShader(_ctoy_frag_shader);
   glDeleteShader(_ctoy_vert_shader);
   glDeleteTextures(1, &_ctoy_texture);
   glfwMakeContextCurrent(NULL);
   glfwTerminate();
   _ctoy_window = NULL;
   m_image_destroy(&_ctoy_buffer_ubyte);
}

static void _ctoy_joy_update(int joy)
{
   const float *axis;
   const unsigned char *button;
   int axis_count, button_count;

   axis = glfwGetJoystickAxes(joy, &axis_count);
   button = glfwGetJoystickButtons(joy, &button_count);

   if (axis) {
      int c = M_MIN(axis_count, CTOY_JOY_AXIS_MAX);
      _ctoy_joystick_axis_count[joy] = c;
      memcpy(_ctoy_joystick_axis[joy], axis, c * sizeof(float));
   }

   if (button) {
      int i, c = M_MIN(button_count, CTOY_JOY_BUTTON_MAX);
      _ctoy_joystick_button_count[joy] = c;
      for (i = 0; i < c; i++) {
         int s0 = _ctoy_joystick_button[joy][i][1];
         int s1 = button[i];
         _ctoy_joystick_button[joy][i][0] = s1 - s0;
         _ctoy_joystick_button[joy][i][1] = s1;
      }
   }
}

static void _ctoy_update(void)
{
   int i;

   /* joystick */
   for (i = 0; i < CTOY_JOY_COUNT; i++)
      _ctoy_joy_update(i);

   /* flush events */
   for (i = 0; i < CTOY_KEY_COUNT; i++)
      _ctoy_button[i][0] = 0;
   for (i = 0; i < CTOY_MOUSE_BUTTON_COUNT; i++)
      _ctoy_mouse_button[i][0] = 0; 
   _ctoy_char_count = 0;
   glfwPollEvents();

   _ctoy_t++;
}

static int _ctoy_oal_init(void)
{
   _ctoy_oal_device = alcOpenDevice(NULL);
   if(! _ctoy_oal_device)
   {
      printf("ERROR OpenAL: unable to create device\n");
        return 0;
   }

    // context
   _ctoy_oal_context = alcCreateContext(_ctoy_oal_device, NULL);
    if(! _ctoy_oal_context)
   {
      printf("ERROR OpenAL: unable to create context\n");
        return 0;
   }

   if(! alcMakeContextCurrent(_ctoy_oal_context))
   {
      printf("ERROR OpenAL: unable to make current context\n");
        return 0;
   }
  
   return 1;
}

static void _ctoy_oal_destroy(void)
{
   alcMakeContextCurrent(NULL);
   alcDestroyContext(_ctoy_oal_context);
   alcCloseDevice(_ctoy_oal_device);
}


int ctoy_get_chars(unsigned int dest[CTOY_CHAR_MAX])
{
   if (_ctoy_char_count > 0)
      memcpy(dest, _ctoy_char_queue, _ctoy_char_count * sizeof(int));
   return _ctoy_char_count;
}

int ctoy_key_press(int key)
{
   return (_ctoy_button[key][0] == 2);
}

int ctoy_key_release(int key)
{
   return (_ctoy_button[key][0] == 1);
}

int ctoy_key_pressed(int key)
{
   return (_ctoy_button[key][1] > 0);
}

int ctoy_mouse_button_press(int button)
{
   return (_ctoy_mouse_button[button][0] == 2);
}

int ctoy_mouse_button_release(int button)
{
   return (_ctoy_mouse_button[button][0] == 1);
}

int ctoy_mouse_button_pressed(int button)
{
   return (_ctoy_mouse_button[button][1] > 0);
}

int ctoy_joystick_present(int joy)
{
   return glfwJoystickPresent(joy);
}

int ctoy_joystick_axis_count(int joy)
{
   return _ctoy_joystick_axis_count[joy];
}

int ctoy_joystick_button_count(int joy)
{
   return _ctoy_joystick_button_count[joy];
}

int ctoy_joystick_button_press(int joy, int button)
{
   return (_ctoy_joystick_button[joy][button][0] > 0);
}

int ctoy_joystick_button_release(int joy, int button)
{
   return (_ctoy_joystick_button[joy][button][0] < 0);
}

int ctoy_joystick_button_pressed(int joy, int button)
{
   return _ctoy_joystick_button[joy][button][1];
}

float ctoy_joystick_axis(int joy, int axis)
{
   return _ctoy_joystick_axis[joy][axis];
}

void ctoy_swap_buffer(struct m_image *image)
{
   if (image) {
      
      if (image->width != _ctoy_tex_width || image->height != _ctoy_tex_height)
         _ctoy_setup_texture(image->width, image->height);

      glBindTexture(GL_TEXTURE_2D, _ctoy_texture);

      if (image->type == M_FLOAT) {
         m_image_float_to_srgb(&_ctoy_buffer_ubyte, image);
         glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _ctoy_buffer_ubyte.width, _ctoy_buffer_ubyte.height, GL_RGB, GL_UNSIGNED_BYTE, _ctoy_buffer_ubyte.data);
      }
      else if (image->type == M_UBYTE) {
         glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->data);
      }
   
      glViewport(0, 0, _ctoy_win_width, _ctoy_win_height);
      _ctoy_draw_texture(_ctoy_texture);
   }
   
   glfwSwapBuffers(_ctoy_window);
}

double ctoy_get_time(void)
{
   return glfwGetTime();
}

void ctoy_window_title(const char *title)
{
   glfwSetWindowTitle(_ctoy_window, title);
   sprintf(_ctoy_title, "%s", title);
}

void ctoy_window_size(int width, int height)
{
   glfwSetWindowSize(_ctoy_window, width, height);
   _ctoy_win_width = width;
   _ctoy_win_height = height;
}

void ctoy_window_fullscreen(int fullscreen)
{
   _ctoy_window_init(_ctoy_title, fullscreen);
}

unsigned long ctoy_t(void)
{
   return _ctoy_t;
}

int ctoy_window_width(void)
{
   return _ctoy_win_width;
}

int ctoy_window_height(void)
{
   return _ctoy_win_height;
}

int ctoy_width(void)
{
   return _ctoy_tex_width;
}

int ctoy_height(void)
{
   return _ctoy_tex_height;
}

float ctoy_mouse_x(void)
{
   return _ctoy_mouse_x;
}

float ctoy_mouse_y(void)
{
   return _ctoy_mouse_y;
}

void ctoy_register_memory(void *memory)
{
   _ctoy_memory = memory;
}

void *ctoy_retrieve_memory(void)
{
   return _ctoy_memory;
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
