// m_image utils

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#include "stb_image.h"

int m_image_load(struct m_image *dest, const char *filename)
{
   int w, h, n;
   unsigned char *data = stbi_load(filename, &w, &h, &n, 0);
   if (data == NULL) {
      printf("ERROR CTOY: unable to read image %s\n", filename);
      return 0;
   }

   m_image_destroy(dest);
   dest->data = data;
   dest->size = w * h * n;
   dest->width = w;
   dest->height = h;
   dest->comp = n;
   dest->type = M_UBYTE;
   return 1;
}

int m_image_load_float(struct m_image *dest, const char *filename)
{
	struct m_image ubi = M_IMAGE_IDENTITY();

	if (! m_image_load(&ubi, filename))
		return 0;
   
	m_image_ubyte_to_float(dest, &ubi);
	m_color_sRGB_to_linear((float*)dest->data, (float*)dest->data, dest->size);
	m_image_destroy(&ubi);
	return 1;
}
