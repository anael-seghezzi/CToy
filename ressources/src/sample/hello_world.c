// Hello World!

#include <ctoy.h>
#include "../util/img_util.c"

#define STAR_COUNT 32
float3 stars[STAR_COUNT];

struct m_image image = M_IMAGE_IDENTITY();
struct m_image buffer = M_IMAGE_IDENTITY();
int fullscreen = 0;

void star_init(void)
{
   int i;
   for (i = 0; i < STAR_COUNT; i++) {
      stars[i].x = m_randf();
      stars[i].y = m_randf();
      stars[i].z = m_randf() * 4;
   }   
}

void star_update(void)
{
   float speed = 0.04;
   int i;
   for (i = 0; i < STAR_COUNT; i++) {
      stars[i].z -= speed;
      if (stars[i].z < 0.1) {
         stars[i].x = m_randf();
         stars[i].y = m_randf();
         stars[i].z = m_randf() * 4;
      }
   }   
}

void star_draw(void)
{
   int i;
   for (i = 0; i < STAR_COUNT; i++) {
      float fx = (stars[i].x - 0.5f) / stars[i].z;
      float fy = (stars[i].y - 0.5f) / stars[i].z;
      int ix = (fx + 0.5f) * buffer.width;
      int iy = (fy + 0.5f) * buffer.height;
      if (ix >= 0 && ix < buffer.width && iy >= 0 && iy < buffer.height) {
         float *pixel = (float *)buffer.data + (iy * buffer.width + ix) * 3;
         if (pixel[0] < 0.5 && pixel[0] < 0.5 && pixel[0] < 0.5) {
            pixel[0] += 0.5;
            pixel[1] += 0.5;
            pixel[2] += 0.5;
         }
      }
   } 
}

void image_draw(void)
{
   float *pixel = (float *)buffer.data;
   int t = ctoy_t();
   float sint = sin(t * 0.05) * 0.5 + 0.5;
   int y, x, i;

   for (y = 0; y < buffer.height; y++) {
      for (x = 0; x < buffer.width; x++) {

         float im_pixel[4];
         float r = (0.15 + sint * 0.15) * m_randf();
         float fx = x;
         float fy = y + sin((t + x) * 0.1) * 4.5;

         m_image_sub_pixel(&image, fx, fy, im_pixel);

         pixel[0] = im_pixel[0] + r * 0.05;
         pixel[1] = im_pixel[1] + r * 0.05;
         pixel[2] = im_pixel[2] + r * 0.05;

         pixel += 3;
      }
   }   
}

void ctoy_begin(void)
{
   printf("Hello World!\n");
   ctoy_window_title("Hello World!");
   ctoy_window_size(512, 512);

   m_image_load_float(&image, "data/hello_world.png");
   m_image_create(&buffer, M_FLOAT, image.width, image.height, 3);
   star_init();
}

void ctoy_end(void)
{
   m_image_destroy(&image);
   m_image_destroy(&buffer);
}

void ctoy_main_loop(void)
{
   // enter / exit fullscreen
   if (ctoy_key_press(CTOY_KEY_F)) {
      fullscreen = !fullscreen;
      ctoy_window_fullscreen(fullscreen);
      if (!fullscreen) ctoy_window_size(512, 512);
   }

   // exit
   if (ctoy_key_press(CTOY_KEY_Q)) {
      exit(EXIT_SUCCESS);
      return;
   }

   // software rendering
   star_update();
   image_draw();
   star_draw();

   ctoy_swap_buffer(&buffer);
}
