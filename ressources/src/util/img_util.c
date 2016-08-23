// m_image utils

int m_image_load_float(struct m_image *dest, const char *filename)
{
	struct m_image ubi = M_IMAGE_IDENTITY();

	if (! ctoy_image_load(&ubi, filename))
		return 0;
   
	m_image_ubyte_to_float(dest, &ubi);
	m_color_sRGB_to_linear((float*)dest->data, (float*)dest->data, dest->size);
	m_image_destroy(&ubi);
	return 1;
}
