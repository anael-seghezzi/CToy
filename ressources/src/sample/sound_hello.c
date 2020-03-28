// OpenAL hello sound

#include <ctoy.h>
#include "../util/al_util.c"

ALuint oal_buffer = 0;
ALuint oal_source = 0;

void sound_begin(void)
{
   alGenBuffers(1, &oal_buffer);
   alGenSources(1, &oal_source);  
}

void sound_end(void)
{
   alDeleteSources(1, &oal_source);
   alDeleteBuffers(1, &oal_buffer);
}

void sound_data(float *data, int size)
{
   alSourcei(oal_source, AL_BUFFER, 0);
   au_buffer_data(oal_buffer, data, size);
   alSourcei(oal_source, AL_BUFFER, oal_buffer);
}

void sound_gain(float gain)
{
   alSourcef(oal_source, AL_GAIN, gain);
}

void sound_looping(int state)
{
   alSourcei(oal_source, AL_LOOPING, state);
}

void sound_play(void)
{
   alSourcePlay(oal_source);
}

void sound_stop(void)
{
   alSourceStop(oal_source);
}

float log2(float n)
{
   return log(n) / 0.69314718246459961;  
}

float round(float d)
{
   return floor(d + 0.5);
}

float sin_wave(float t)
{
   return sin(2.0*M_PI * t);
}

float square_wave(float t)
{
   return sin(2.0*M_PI * t) > 0 ? 1 : -1;
}

float saw_wave(float t)
{
   return 2 * (t - floor(t + 0.5));
}

float tri_wave(float t)
{
   return 1.0 - 4.0 * fabs(round(t - 0.25) - (t - 0.25));
}

float noise(float t)
{
   float v = sin(123523.9898 * t) * 43758.5453;
   return v - floor(v);
}

void synthesize(void)
{
   struct m_image sound = M_IMAGE_IDENTITY();

   float *samples;
   int sample_rate = 44100;
   float half_freq = 0.5 / sample_rate;
   float duration = 1.0;
   int frames = duration * sample_rate;
   int i;

   m_image_create(&sound, M_FLOAT, frames, 1, 2);
   samples = (float *)sound.data;

   for (i = 0; i < sound.size; i+=2) {

      float f = (float)i / (sound.size - 2);
      float t = half_freq * i;
      float fade = M_MIN(1, f*2) * M_MIN(1, (1.0-f)*2);

      int in = (int)(t * 8);
      float note = (in + 1) * 0.5;

      float s1 = sin_wave(t * 4.0);
      float s2 = tri_wave(t * note * 200);
      float s3 = tri_wave(t * note * 400);

      samples[0] = (s1 * s2) * fade;
      samples[1] = (s1 * s3) * fade;

      samples += 2;
   }

   m_image_gaussian_blur(&sound, &sound, 8, 0);

   sound_stop();
   sound_data((float *)sound.data, sound.size);
   sound_looping(0);
   sound_play();

   m_image_destroy(&sound);
}

void ctoy_begin(void)
{
   printf("<sample sound_hello>\n");
   sound_begin();
   synthesize();
}

void ctoy_end(void)
{
   sound_end();
}

void ctoy_main_loop()
{
   ctoy_sleep(0, 1000000);
}
