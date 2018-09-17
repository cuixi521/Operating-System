#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>    
#include <sys/wait.h>


int Collaz(int i)
{
	pid_t fpid;
	fpid = fork();

	if (fpid < 0)
	{
		printf("Error in fork!");
		return -1;
	}
	else if (fpid == 0)
	{
		if (i == 1) {
			printf("%d ", i);
			return 0;
		}

		fprintf(stdout, "%d, ", i);
		fflush(stdout);

		if (i % 2 == 0) {
			i = i / 2;
		}
		else {
			i = 3 * i + 1;
		}
		
		Collaz(i);
		//printf("childEnd%d",i);
	}
	else
	{
		//printf("father%d",i);
		wait(NULL);
	}
}

int main()
{
	int n;
	printf("Enter an integer: ");
	scanf("%d", &n);
	
	Collaz(n);
	return 0;
}


