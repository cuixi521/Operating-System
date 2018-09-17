#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include <time.h>
#include <sys/types.h>    
#include <sys/wait.h>
#include<semaphore.h>
#include <math.h>


#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int request_resources(int customer_num, int request[]);
int release_resources(int customer_num, int request[]);
int safe();
void printM(int M[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES], char *name);

pthread_mutex_t Mymutex;
//pthread_mutex_t mutexalloc;
//pthread_mutex_t mutexneed;

typedef struct {
	int cid;
	pthread_t tid;
} Array;

void *customer(void *cid)                                          //customer request resourses
{
	int request[NUMBER_OF_RESOURCES];
	int *p = (int*)cid;
	int c_num = *p;     //customer number;
	for (int k = 0; k < 5; k++) {

		for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
		{
			request[j] = rand() % (need[c_num][j] + 1);
		}
		pthread_mutex_lock(&Mymutex);
		int rq = request_resources(c_num, request);
		//printf("request%d\n", rq);
		if (rq < 0) {                                         //request unsuccessful
			printf("customer%d request: [ ", c_num);
			for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
				printf("%d ", request[i]);
			}
			printf("] unsuccessful\n");
			printf("Available: ");
			//fflush(stdout);
			for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
			{
				printf("%d ", available[i]);
			}
			printf("\n");
		}

		pthread_mutex_unlock(&Mymutex);

		if(rq==0) {                                        //if request successful then release resources
			sleep(1);
			pthread_mutex_lock(&Mymutex);
			release_resources(c_num, request);
			pthread_mutex_unlock(&Mymutex);
		}
		
		
	}
}

int request_resources(int customer_num, int request[]) {

	for (int i = 0; i<NUMBER_OF_RESOURCES; i++)
	{
		if (request[i] > need[customer_num][i]) {                   //request unsuccessful
			//pthread_mutex_unlock(&Mymutex);
			return -1;
		}
		else if (request[i] > available[i]) {                       //request unsuccessful
			//pthread_mutex_unlock(&Mymutex);
			return -1;
		}
		else 
		{
			allocation[customer_num][i] += request[i];
			available[i] -= request[i];
			need[customer_num][i] -= request[i];
		}
	}

	int s = safe();
	//printf("safe:%d", s);
	if (s != 1)
	{
		for (int j = 0; j<NUMBER_OF_RESOURCES; j++)
		{
			allocation[customer_num][j] -= request[j];
			available[j] += request[j];
			need[customer_num][j] += request[j];
		}
		return -1;
	}

	printf("customer%d request: [ ", customer_num);
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
		printf("%d ", request[i]);
	}
	printf("] successful\n");

	printM(allocation, "allocation");
	printM(need, "need");

	printf("Available: ");
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		printf("%d ", available[i]);
	}
	printf("\n");

	//pthread_mutex_unlock(&Mymutex);
	return 0;
}

int release_resources(int customer_num, int request[])
{
	//pthread_mutex_lock(&Mymutex);
	for ( int i = 0; i<NUMBER_OF_RESOURCES; i++)
	{
		allocation[customer_num][i] -= request[i];

		available[i] += request[i];

		need[customer_num][i] = maximum[customer_num][i] - allocation[customer_num][i];
	}
	//pthread_mutex_unlock(&Mymutex);
	return 1;
}

int safe() {                                                                 //cheack safe algorithem
	int work[NUMBER_OF_RESOURCES], finish[NUMBER_OF_CUSTOMERS];
	int count = 0;
	int flag = 0;
	for (int j = 0; j<NUMBER_OF_RESOURCES; j++)
	{
		work[j] = available[j];
	}

	for (int i = 0; i<NUMBER_OF_CUSTOMERS; i++)
	{
		 finish[i] = 0;                     //0- False 1 - True
	}

	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
	{
		for (int k = 0; k < NUMBER_OF_CUSTOMERS; k++) {
			if (finish[k] == 0)
			{
				for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
				{
					if (need[k][j] > work[j]) { flag=0; }
					else if(j==NUMBER_OF_RESOURCES-1){
						finish[k] = 1;
						flag = 1;
						count++;
					}
				}
				if (flag == 1) {
					for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
						work[j] += allocation[k][j];
					}
				}
			}
		}
		if (count == NUMBER_OF_CUSTOMERS) {
			return 1;}
	}
	return 0;
}

void initial_Max_Allocation()
{
	srand(time(NULL));
	for (int i = 0; i<NUMBER_OF_CUSTOMERS; i++)
	{
		for (int j = 0; j<NUMBER_OF_RESOURCES; j++)
		{
			maximum[i][j] = rand() % (available[j]+1);
			allocation[i][j] = 0;
			need[i][j] = maximum[i][j] - allocation[i][j];
		}
	}
}

void printM(int M[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES], char *name)
{
	printf("%s matrix:\n", name);
	for (int i = 0; i<NUMBER_OF_CUSTOMERS; i++)
	{
		for (int j = 0; j<NUMBER_OF_RESOURCES; j++){ printf("%d ", M[i][j]); }		
		printf("\n");
	}
}

int main(int argc, char  *argv[])
{
	if (argc < 4) {
		printf("Parameter absent!");
		return -1;
	}

	printf("Available: ");
	for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		available[i] = atoi(argv[i+1]);
		printf("%d ", available[i]);
	}
	printf("\n");
	initial_Max_Allocation();
	printM(maximum, "maximum");
	printM(allocation, "allocation");
	printM(need, "need");

	Array *tArray;
	int size = (int)NUMBER_OF_CUSTOMERS;
	tArray = (Array *)malloc(sizeof(Array) * size);

	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		tArray[i].cid = i;
		pthread_create(&tArray[i].tid, NULL, customer, &tArray[i].cid);
	}

	for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		pthread_join(tArray[i].tid, NULL);
	}
	return 0;
}