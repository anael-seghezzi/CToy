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

#ifndef CTOY_H
#define CTOY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <float.h>
#include <GLES2/gl2.h>
#include <AL/al.h>
#include "m_math.h"
#include "m_image.h"
#include "m_color.h"
#include "m_dist.h"
#include "m_raster.h"

#define CTOY_CHAR_MAX 256

/* custom calls */
void ctoy_begin(void);
void ctoy_main_loop(void);
void ctoy_end(void);

/* API */
unsigned long ctoy_t(void);
int ctoy_window_width(void);
int ctoy_window_height(void);
float ctoy_mouse_x(void);
float ctoy_mouse_y(void);
double ctoy_get_time(void);
void ctoy_sleep(long sec, long nsec);

void ctoy_window_size(int width, int height);
void ctoy_window_title(const char *title);
void ctoy_window_fullscreen(int state);
void ctoy_swap_buffer(struct m_image *src);

int ctoy_key_press(int key);
int ctoy_key_release(int key);
int ctoy_key_pressed(int key);

int ctoy_mouse_button_press(int button);
int ctoy_mouse_button_release(int button);
int ctoy_mouse_button_pressed(int button);

int ctoy_joystick_present(int joy);
int ctoy_joystick_axis_count(int joy);
int ctoy_joystick_button_count(int joy);
int ctoy_joystick_button_press(int joy, int button);
int ctoy_joystick_button_release(int joy, int button);
int ctoy_joystick_button_pressed(int joy, int button);
float ctoy_joystick_axis(int joy, int axis);

int ctoy_get_chars(unsigned int dest[CTOY_CHAR_MAX]);

void ctoy_register_memory(void *memory);
void *ctoy_retrieve_memory(void);

/* keys (glfw mapping) */
#define CTOY_KEY_SPACE              32
#define CTOY_KEY_APOSTROPHE         39
#define CTOY_KEY_COMMA              44
#define CTOY_KEY_MINUS              45
#define CTOY_KEY_PERIOD             46
#define CTOY_KEY_SLASH              47
#define CTOY_KEY_0                  48
#define CTOY_KEY_1                  49
#define CTOY_KEY_2                  50
#define CTOY_KEY_3                  51
#define CTOY_KEY_4                  52
#define CTOY_KEY_5                  53
#define CTOY_KEY_6                  54
#define CTOY_KEY_7                  55
#define CTOY_KEY_8                  56
#define CTOY_KEY_9                  57
#define CTOY_KEY_SEMICOLON          59
#define CTOY_KEY_EQUAL              61
#define CTOY_KEY_A                  65
#define CTOY_KEY_B                  66
#define CTOY_KEY_C                  67
#define CTOY_KEY_D                  68
#define CTOY_KEY_E                  69
#define CTOY_KEY_F                  70
#define CTOY_KEY_G                  71
#define CTOY_KEY_H                  72
#define CTOY_KEY_I                  73
#define CTOY_KEY_J                  74
#define CTOY_KEY_K                  75
#define CTOY_KEY_L                  76
#define CTOY_KEY_M                  77
#define CTOY_KEY_N                  78
#define CTOY_KEY_O                  79
#define CTOY_KEY_P                  80
#define CTOY_KEY_Q                  81
#define CTOY_KEY_R                  82
#define CTOY_KEY_S                  83
#define CTOY_KEY_T                  84
#define CTOY_KEY_U                  85
#define CTOY_KEY_V                  86
#define CTOY_KEY_W                  87
#define CTOY_KEY_X                  88
#define CTOY_KEY_Y                  89
#define CTOY_KEY_Z                  90
#define CTOY_KEY_LEFT_BRACKET       91
#define CTOY_KEY_BACKSLASH          92
#define CTOY_KEY_RIGHT_BRACKET      93
#define CTOY_KEY_GRAVE_ACCENT       96
#define CTOY_KEY_WORLD_1            161
#define CTOY_KEY_WORLD_2            162
#define CTOY_KEY_ESCAPE             256
#define CTOY_KEY_ENTER              257
#define CTOY_KEY_TAB                258
#define CTOY_KEY_BACKSPACE          259
#define CTOY_KEY_INSERT             260
#define CTOY_KEY_DELETE             261
#define CTOY_KEY_RIGHT              262
#define CTOY_KEY_LEFT               263
#define CTOY_KEY_DOWN               264
#define CTOY_KEY_UP                 265
#define CTOY_KEY_PAGE_UP            266
#define CTOY_KEY_PAGE_DOWN          267
#define CTOY_KEY_HOME               268
#define CTOY_KEY_END                269
#define CTOY_KEY_CAPS_LOCK          280
#define CTOY_KEY_SCROLL_LOCK        281
#define CTOY_KEY_NUM_LOCK           282
#define CTOY_KEY_PRINT_SCREEN       283
#define CTOY_KEY_PAUSE              284
#define CTOY_KEY_F1                 290
#define CTOY_KEY_F2                 291
#define CTOY_KEY_F3                 292
#define CTOY_KEY_F4                 293
#define CTOY_KEY_F5                 294
#define CTOY_KEY_F6                 295
#define CTOY_KEY_F7                 296
#define CTOY_KEY_F8                 297
#define CTOY_KEY_F9                 298
#define CTOY_KEY_F10                299
#define CTOY_KEY_F11                300
#define CTOY_KEY_F12                301
#define CTOY_KEY_F13                302
#define CTOY_KEY_F14                303
#define CTOY_KEY_F15                304
#define CTOY_KEY_F16                305
#define CTOY_KEY_F17                306
#define CTOY_KEY_F18                307
#define CTOY_KEY_F19                308
#define CTOY_KEY_F20                309
#define CTOY_KEY_F21                310
#define CTOY_KEY_F22                311
#define CTOY_KEY_F23                312
#define CTOY_KEY_F24                313
#define CTOY_KEY_F25                314
#define CTOY_KEY_KP_0               320
#define CTOY_KEY_KP_1               321
#define CTOY_KEY_KP_2               322
#define CTOY_KEY_KP_3               323
#define CTOY_KEY_KP_4               324
#define CTOY_KEY_KP_5               325
#define CTOY_KEY_KP_6               326
#define CTOY_KEY_KP_7               327
#define CTOY_KEY_KP_8               328
#define CTOY_KEY_KP_9               329
#define CTOY_KEY_KP_DECIMAL         330
#define CTOY_KEY_KP_DIVIDE          331
#define CTOY_KEY_KP_MULTIPLY        332
#define CTOY_KEY_KP_SUBTRACT        333
#define CTOY_KEY_KP_ADD             334
#define CTOY_KEY_KP_ENTER           335
#define CTOY_KEY_KP_EQUAL           336
#define CTOY_KEY_LEFT_SHIFT         340
#define CTOY_KEY_LEFT_CONTROL       341
#define CTOY_KEY_LEFT_ALT           342
#define CTOY_KEY_LEFT_SUPER         343
#define CTOY_KEY_RIGHT_SHIFT        344
#define CTOY_KEY_RIGHT_CONTROL      345
#define CTOY_KEY_RIGHT_ALT          346
#define CTOY_KEY_RIGHT_SUPER        347
#define CTOY_KEY_MENU               348

/* mouse buttons */
#define CTOY_MOUSE_BUTTON_1         0
#define CTOY_MOUSE_BUTTON_2         1
#define CTOY_MOUSE_BUTTON_3         2
#define CTOY_MOUSE_BUTTON_4         3
#define CTOY_MOUSE_BUTTON_5         4
#define CTOY_MOUSE_BUTTON_6         5
#define CTOY_MOUSE_BUTTON_7         6
#define CTOY_MOUSE_BUTTON_8         7
#define CTOY_MOUSE_BUTTON_LEFT      CTOY_MOUSE_BUTTON_1
#define CTOY_MOUSE_BUTTON_RIGHT     CTOY_MOUSE_BUTTON_2
#define CTOY_MOUSE_BUTTON_MIDDLE    CTOY_MOUSE_BUTTON_3

/* joysticks */
#define CTOY_JOYSTICK_1             0
#define CTOY_JOYSTICK_2             1
#define CTOY_JOYSTICK_3             2
#define CTOY_JOYSTICK_4             3
#define CTOY_JOYSTICK_5             4
#define CTOY_JOYSTICK_6             5
#define CTOY_JOYSTICK_7             6
#define CTOY_JOYSTICK_8             7
#define CTOY_JOYSTICK_9             8
#define CTOY_JOYSTICK_10            9
#define CTOY_JOYSTICK_11            10
#define CTOY_JOYSTICK_12            11
#define CTOY_JOYSTICK_13            12
#define CTOY_JOYSTICK_14            13
#define CTOY_JOYSTICK_15            14
#define CTOY_JOYSTICK_16            15

#endif
