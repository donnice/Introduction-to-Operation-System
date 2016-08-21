#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// page 75

#define NUMBER_OF_THREADS 10

void *print_hello_world(void *tid)
{
	printf("Hello world! Greeting from Thread %d\n",tid);
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_t threads[NUMBER_OF_THREADS];
	int status,i;

	for(i = 0; i < NUMBER_OF_THREADS;i++)
	{
		printf("Main here. Creating thread %d\n",i);
		status = pthread_create(&threads[i],NULL,print_hello_world,(void*)i);

		if(status!=0)
		{
			printf("Oops!");
			exit(-1);
		}

	}
	
	exit(NULL);

	return 0;
}
