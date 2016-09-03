// OpenAL utils

void au_buffer_data16(ALuint buffer, short *data, int size)
{
   alBufferData(buffer, AL_FORMAT_STEREO16, data, size * 2, 44100);
}

void au_buffer_data(ALuint buffer, float *data, int size)
{
   short *sdata = (short *)malloc(size * sizeof(short));
   int i;

   for (i = 0; i < size; i++)
      sdata[i] = M_CLAMP(data[i], -1, 1) * 32760;
   
   au_buffer_data16(buffer, sdata, size);
   free(sdata);
}
