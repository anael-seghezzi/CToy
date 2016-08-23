#include <dirent.h>

void ctoy_begin(void)
{
	printf("<test dirent>\n");

	DIR *pdir = opendir("data");
	if (!pdir) {
		printf("nope\n");
	}
	else {
		struct dirent * pent = NULL;
    	while ((pent = readdir(pdir)))
			printf("%d: %s\n", pent->d_type, pent->d_name);
		closedir(pdir);
	}
}

void ctoy_main_loop(void)
{
	ctoy_sleep(0, 1000000);
}

void ctoy_end(void)
{}
