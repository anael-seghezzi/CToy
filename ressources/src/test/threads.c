#include <sys/stat.h>

thrd_t t;
int t_running = 1;
int t_counter = 0;

int my_thread(void *data)
{
	while (t_running) {
		t_counter++;
		thrd_yield();
	}
	return 0;
}

void ctoy_begin(void)
{
	printf("<test threads>\n");

	if (thrd_create(&t, my_thread, NULL) != thrd_success) {
		printf("threads error\n");
		return;
	}
	
	ctoy_sleep(1, 0);
	
	t_running = 0;
	thrd_join(t, NULL);
	
	printf("t_counter = %d\n", t_counter);
}

void ctoy_main_loop(void)
{
	ctoy_sleep(0, 1000000);
}

void ctoy_end(void)
{}
