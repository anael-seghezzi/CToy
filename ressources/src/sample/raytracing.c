/*======================================================================
 CToy Raytracing sample
------------------------------------------------------------------------
 Copyright (c) 2015-2017 Anael Seghezzi <www.maratis3d.com>

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

#define USE_NOISE /* comment to disable 3d noise (simple sphere raytracing) */

static struct m_image test_buffer = M_IMAGE_IDENTITY();

/* approximative noise */
static struct m_image rand_image = M_IMAGE_IDENTITY();
static struct m_image tmp = M_IMAGE_IDENTITY();

void init_noise(void)
{
	int i;
	m_image_create(&rand_image, M_FLOAT, 16, 16, 2);
	for (i = 0; i < rand_image.size; i++)
		((float *)rand_image.data)[i] = m_randf();
}

void destroy_noise(void)
{
	m_image_destroy(&rand_image);
}

float fast_noise(float x, float y, float z)
{
	float values[2];
	float i = z - floorf(z);
	m_image_sub_pixel(&rand_image, x + z + 8, y + z + 8, values);
	return values[0] * (1.0f - i) + values[1] * i;
}

#define GET_RAY(ray, px, py, pz, hw, hh, ratio)\
{\
	float3 pt = {((float)px - hw) / hw, (-((float)py - hh) / hh) * ratio, pz};\
	float ptl = 1.0f / M_LENGHT3(pt);\
	ray.x = pt.x * ptl;\
	ray.y = pt.y * ptl;\
	ray.z = pt.z * ptl;\
}

static void draw(void)
{
	float *data = (float *)test_buffer.data;
	int w = test_buffer.width;
	int h = test_buffer.height;
	int y;
	int test_t = ctoy_t();
	
	float3 sphere_pos;
	float3 light_dir;
	float z_near = 1e-4;
	float ambient = 0.15f;
	float sphere_radius2;
	float sphere_tex_unit;
	float hw = w * 0.5f;
	float hh = h * 0.5f;
	float ratio = (float)ctoy_window_height() / (float)ctoy_window_width();
	
	/* light */
	light_dir.x = 0.5f;
	light_dir.y = 0.25f;
	light_dir.z = -0.5f;
	
	/* sphere */
	sphere_radius2 = 150;
	sphere_pos.x = cosf(test_t * 0.025f) * 20.0f;
	sphere_pos.y = 0.0f;
	sphere_pos.z = 50.0f + (sinf(test_t * 0.025f) + 1.0f) * 70.0f;
	sphere_tex_unit = 0.25f;

	/* clear */
	memset(test_buffer.data, 0, test_buffer.size * sizeof(float));
	
	/* raytrace */
	#pragma omp parallel for schedule(dynamic, 8)
	for (y = 0; y < h; y++) {
		
		float *pixel = data + y * w * 3;
		int x;
		
		for (x = 0; x < w; x++) {
			
			float3 origin = {0, 0, 0};
			float3 ray, march_dir;
			float march_step;
			float idist, dist = 0, Z = 1e20;
			
			/* get ray from pixel position */
			GET_RAY(ray, x, y, 1.35f, hw, hh, ratio);

			march_step = 0.25f;
			march_dir.x = ray.x * march_step;
			march_dir.y = ray.y * march_step;
			march_dir.z = ray.z * march_step;
			
			/* sphere */
			m_3d_ray_sphere_intersection_in_out(&origin, &ray, &sphere_pos, sphere_radius2, &dist, &idist);
			if (dist > z_near) {
				
				if (dist < Z) {
					
					float3 rd = {ray.x * dist, ray.y * dist, ray.z * dist};
					float3 pos = {origin.x + rd.x, origin.y + rd.y, origin.z + rd.z};
					
					/* simple sphere */
					#ifndef USE_NOISE
					{
						float3 normal;
						float diffuse;
						M_SUB3(normal, pos, sphere_pos);
						M_NORMALIZE3(normal, normal);
						diffuse = M_DOT3(normal, light_dir);
						diffuse = M_MAX(0, diffuse);
						pixel[0] = ambient + diffuse;
						pixel[1] = ambient + diffuse;
						pixel[2] = ambient + diffuse;
						Z = dist;
					}
					/* volumetric ray marching inside a sphere (perlin noise test) */
					#else
					{
						float3 march = pos; /* starting at sphere intersection */
						int i;

						for (i = 0; i < 256; i++) {

							float3 vcoord = {
								(march.x - sphere_pos.x) * sphere_tex_unit,
								(march.y - sphere_pos.y) * sphere_tex_unit,
								(march.z - sphere_pos.z) * sphere_tex_unit
							};
	
							float perlin = fast_noise(vcoord.x, vcoord.y, vcoord.z);
							if (perlin > 0.6f) {

								/* render */
								float3 normal;
								float diffuse;

								if (i == 0) {
									/* sphere normal */
									M_SUB3(normal, pos, sphere_pos);
									M_NORMALIZE3(normal, normal);
								} else {
									/* volume normal */
									normal.x = perlin - fast_noise(vcoord.x + 0.0001f, vcoord.y, vcoord.z);
									normal.y = perlin - fast_noise(vcoord.x, vcoord.y + 0.0001f, vcoord.z);
									normal.z = perlin - fast_noise(vcoord.x, vcoord.y, vcoord.z + 0.0001f);
									M_NORMALIZE3(normal, normal);
								}

								diffuse = M_DOT3(normal, light_dir);
								diffuse = M_MAX(0, diffuse);
								pixel[0] = ambient + diffuse;
								pixel[1] = ambient + diffuse;
								pixel[2] = ambient + diffuse;
								Z = dist;
								break;
							}

							/* march */
							M_ADD3(march, march, march_dir);
							dist += march_step;

							if (dist > idist) /* out of the sphere */
								break;
						}
					}
					#endif
				}
			}
			
			pixel += 3;
		}
	}
}

void ctoy_begin(void)
{
   printf("Raytracing\n");
   ctoy_window_title("Raytracing");
   ctoy_window_size(512, 512);

   m_image_create(&test_buffer, M_FLOAT, 256, 256, 3);
   init_noise();
}

void ctoy_end(void)
{
   m_image_destroy(&test_buffer);
   destroy_noise();
}

void ctoy_main_loop(void)
{
   draw();
   ctoy_swap_buffer(&test_buffer);
}
