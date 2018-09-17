
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pid.h"

int main()
{
	allocate_map();
	int pid = allocate_pid();
	int tid = allocate_pid();
	fprintf(stdout, "Allocated PID: %d \n", pid);
	printf("Allocated PID: %d \n", tid);
	release_pid(pid);
	int sid = allocate_pid();
	printf("Allocated PID: %d \n", sid);
	return 0;
}