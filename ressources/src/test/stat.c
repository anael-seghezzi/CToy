#include <sys/stat.h>

void ctoy_begin(void)
{
	printf("<test stat>\n");

	struct stat buffer;
	if (stat("data/hello_world.png", &buffer) == 0) {

		int64_t t = buffer.st_mtime;
		printf("%d %d\n", t, buffer.st_size);
	}
}

void ctoy_main_loop(void)
{
	ctoy_sleep(0, 1000000);
}

void ctoy_end(void)
{}
