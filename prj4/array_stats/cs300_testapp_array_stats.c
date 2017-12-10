#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <sys/types.h>

#define _ARRAY_STATS_SYSCALL_NUMBER 332

#if _LP64 == 1
#define _CS300_TEST_ 330
#else
#define _CS300_TEST_ 360
#endif

struct array_stats{
	long min;
	long max;
	long sum;
};

struct array_stats stats = {100,0,0};

long data[4] = {1,20,50,100};
int size = 4;


int main(int argc, char const *argv[])
{
	printf("\nDiving to kernel level\n\n");
	int result = syscall(_ARRAY_STATS_SYSCALL_NUMBER, &stats,data,size);
	printf("\nRising to user level w/ result = %d\n\n", result);
	return 0;
}
