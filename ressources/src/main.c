
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

#elif CTOY_SAMPLE == 5
#include "sample/raytracing.c"

#elif CTOY_SAMPLE == 6
#include "sample/voronoi.c"

#elif CTOY_SAMPLE == 7
#include "sample/imgui_hello.c"

#elif CTOY_SAMPLE == 8
#include "sample/bomber.c"

#endif
