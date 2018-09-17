#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include <time.h>
#include <sys/types.h>    
#include <sys/wait.h>
#include<semaphore.h>

int StudentID;
int help;
int students;
int count = 0;
sem_t seat;
sem_t empty;
sem_t mutex;
sem_t full;
sem_t available;

typedef struct {
	int sid;
	pthread_t tid;
	int need;
} Array;

void *Student(void *sid)
{
	int *p = (int*)sid;
	int id = *p;

	for (int i = 0; i < help; i++)
	{
		int r = 100 + rand() % 10000;            //define the returning time of student
		//int sit = sem_trywait(&seat);
		//if (sit < 0) {
		//	usleep(r);
		//	i--;
		//	continue;
		//}	
		if(count==0){                           //cheack the chairs are all free
		int w = sem_trywait(&empty);            //cheack if one can enter office and seek help
		if (w < 0) {
			int sit = sem_trywait(&seat);       //cheack if one can sit on the chairs 
			if (sit < 0) {
				usleep(r);
				i--;
				continue;
			}
			count++;
			printf("%d sit on chair\n", id);
			sem_wait(&empty);                   //after siting on chair waiting chance to enter office
			sem_wait(&mutex);

			printf("%d enter\n", id);
			StudentID = id;

			sem_post(&mutex);
			sem_post(&full);
			sem_post(&seat);                    //student leave chair
			count--;
			sem_wait(&available);
			continue;
		}

		printf("%d office\n", id);
		sem_wait(&mutex);

		StudentID = id;                         //under one thread TA help the student who enter the office

		sem_post(&mutex);
		sem_post(&full);
		sem_wait(&available);
	}
		else {
			int sit = sem_trywait(&seat);          
			if (sit < 0) {
				usleep(r);
				i--;
				continue;
			}
			count++;
			printf("%d sit on chair\n", id);
			sem_wait(&empty);
			sem_wait(&mutex);
			printf("%d enter\n", id);
			StudentID = id;

			sem_post(&mutex);
			sem_post(&full);
			sem_post(&seat);
			count--;
			sem_wait(&available);
			continue;
		}
	}

}

int main( int argc, char  *argv[])
{
	if (argc < 4) {
		printf("Parameter absent!");
		return -1;
	}
	
	students = atoi(argv[1]);
	int chair = atoi(argv[2]);
	help = atoi(argv[3]);
	
	Array *tArray;
	tArray = (Array *)malloc(sizeof(Array) * students);

	sem_init(&empty, 0, 1);
	sem_init(&full, 0, 0);
	sem_init(&mutex, 0, 1);
	sem_init(&seat, 0, chair);
	sem_init(&available, 0, 0);

	for (int i = 0; i < students; i++) {
		tArray[i].sid = i;
		pthread_create(&tArray[i].tid, NULL, Student, &tArray[i].sid);
	}

	int times = students*help;
	for (int i = 0; i < times; i++) {                //TA help students
		
		sem_wait(&full);
		sem_wait(&mutex);

		printf("Now help Student%d\n", StudentID);
		tArray[StudentID].need++;

		if (tArray[StudentID].need == help) { printf("----Student%d finished----\n", StudentID); }
		
		sem_post(&mutex);
		sem_post(&empty);                           
		sem_post(&available);                       //TA finished help this student

	}

	for (int i = 0; i < students; i++) {
		pthread_join(tArray[i].tid, NULL);
	}

	return 0;
}

