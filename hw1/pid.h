#pragma once
#define MIN_PID 300
#define MAX_PID 5000

struct pid_struct
{
	int pid;
	bool bitmap;
};

pid_struct pidA[4700];

int allocate_map();
int allocate_pid();
void release_pid(int pid);


