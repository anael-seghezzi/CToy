#include <ctoy.h>

#define CTOY_SAMPLE 1 // change me

// samples
#if CTOY_SAMPLE == 1
#include "sample/hello_world.c"

#elif CTOY_SAMPLE == 2
#include "sample/triangle_hello.c"

#elif CTOY_SAMPLE == 3
#include "sample/sound_hello.c"

#elif CTOY_SAMPLE == 4
#include "sample/libtcc_hello.c"
#endif
