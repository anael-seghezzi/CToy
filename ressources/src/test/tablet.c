// pen tablet test

void ctoy_begin(void)
{}

void ctoy_main_loop(void)
{
   struct ctoy_pen_data pen_data[CTOY_PEN_DATA_MAX];
   int pen_data_size = ctoy_get_pen_data(pen_data);
   int i;

   for (i = 0; i < pen_data_size; i++) {
      printf("%f %f %f %f %f %f\n", pen_data[i].x, pen_data[i].y, pen_data[i].z, pen_data[i].pressure, pen_data[i].pitch, pen_data[i].yaw);
   }
}

void ctoy_end(void)
{}
