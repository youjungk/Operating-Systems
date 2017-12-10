#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <sys/types.h>

#define _PROCESS_ANCESTORS_SYSCALL_NUMBER 333
#define ANCESTOR_NAME_LEN 16

#if _LP64 == 1
#define _CS300_TEST_ 330
#else
#define _CS300_TEST_ 360
#endif

struct process_info {
	long pid;                     /* Process ID */
	char name[ANCESTOR_NAME_LEN]; /* Program name of process */
	long state;                   /* Current process state */
	long uid;                     /* User ID of process owner */
	long nvcsw;                   /* # voluntary context switches */
	long nivcsw;                  /* # involuntary context switches */
	long num_children;            /* # children process has */
	long num_siblings;            /* # sibling process has */
};

void p_test(struct process_info *info, int size){
	struct process_info process;
	for (int i=0; i<size; i++){
		process = info[i];
		printf("pid: %ld\n", process.pid);
		printf("name: %s\n", process.name);
		printf("state: %ld\n", process.state);
		printf("uid: %ld\n", process.uid);
		printf("nvcsw: %ld\n", process.nvcsw);
		printf("nivcsw: %ld\n", process.nivcsw);
		printf("num_children: %ld\n", process.num_children);
		printf("num_siblings: %ld\n", process.num_siblings);	
	}
}


int main(int argc, char const *argv[])
{
	long size = 3;
	long *num = 0;
	int result;
	printf("\nDiving to kernel level\n\n");

	struct process_info* info;
	result = syscall(_PROCESS_ANCESTORS_SYSCALL_NUMBER, info, size,num);
	p_test(info,size);
	printf("\nRising to user level w/ result = %d\n\n", result);
	return 0;
}