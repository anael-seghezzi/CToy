#include <sys/stat.h>

void ctoy_begin(void)
{
	float4 v1, v2, v3;

	printf("<test asm>\n");

	v1.x = 0.1;
	v1.y = 0.2;
	v1.z = 0.4;
	v1.w = 0.3;

	v2.x = 0.11;
	v2.y = 0.0;
	v2.z = 0.01;
	v2.w = 0.04;

	asm volatile (
		"movups %0, %%xmm0;"
		"movups %1, %%xmm1;"
		"addps %%xmm1, %%xmm0;"
		"movups %%xmm0, %2"
		:
		: "g" (v1), "g" (v2), "g" (v3)
		: "memory"
	);

	printf("sse float4 add4 : %f %f %f %f\n", v3.x, v3.y, v3.z, v3.w);
	printf("expected result : %f %f %f %f\n", v1.x+v2.x, v1.y+v2.y, v1.z+v2.z, v1.w+v2.w);
}

void ctoy_main_loop(void)
{
	ctoy_sleep(0, 1000000);
}

void ctoy_end(void)
{}
