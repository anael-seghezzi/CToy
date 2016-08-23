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

#include "ctoy.c"
#include "../../../src/main.c"


static void _ctoy_main_loop(void)
{
   ctoy_main_loop();
   _ctoy_update();
}

int main(int argc, char **argv)
{
#ifndef __EMSCRIPTEN__
   char dir[256];
   _ctoy_get_directory(dir, argv[0]);
   _ctoy_set_working_dir(dir);
#endif

   /* openal */
   _ctoy_oal_init();

   /* window */
   if (! _ctoy_create("CTOY", 512, 512)) {
      printf("ERROR CTOY: could not create window\n");
      _ctoy_oal_destroy();
      return EXIT_FAILURE;
   }

   /* run */
   ctoy_begin();

#ifdef __EMSCRIPTEN__
   emscripten_set_main_loop(_ctoy_main_loop, 0, 1);
#else
   while (_ctoy_state) {
      _ctoy_main_loop();
      thrd_yield();
   }
#endif

   ctoy_end();

   /* clear */
   _ctoy_destroy();
   _ctoy_oal_destroy();
   return EXIT_SUCCESS;
}
