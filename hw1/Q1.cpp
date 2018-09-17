#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pid.h"

int allocate_map()                                 
{
	int i = 0;
	for (i = 0; i <= 4700; i++)
	{
		pidA[i].pid = i + MIN_PID;
		pidA[i].bitmap = 0;
	}

	if (i == 4700)
	{ 
		return 1; 
	}	
	else
	{ 
		return -1; 
	}
		
}

int allocate_pid()                                  
{
	for (int i = 0; i <= 4700; i++)
	{
		if (pidA[i].bitmap == 0)
		{
			pidA[i].pid = i + MIN_PID;
			pidA[i].bitmap = 1;
			return pidA[i].pid;
			break;
		}
	}

	return -1;
}

void release_pid(int pid)                               
{
	for (int i = 0; i <= 4700; i++)
	{
		if (pidA[i].pid == pid)
		{
			pidA[i].bitmap = 0;
		}
	}
}