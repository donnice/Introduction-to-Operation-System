// page 91
#include <stdio.h>
#include <pthread.h>
#define MAX 100000000
pthread_mutex_t the_mutex;
pthread_cond_t condc,condp;
int buffer = 0;					/* Buffer for both producer and consumer */

void *producer(void *ptr)
{
	int i;

	for(i = 1; i<=MAX;i++)
	{
		pthread_mutex_lock(&the_mutex);		/* Mutex the buffer */
		while(buffer !=0) pthread_cond_wait(&condp, &the_mutex);
		buffer=i;							/* put data in the buffer */
		pthread_cond_signal(&condc);		/* wake the consumer */
		pthread_mutex_unlock(&the_mutex);	/* release the buffer */
	}
	pthread_exit(0);
}

void *consumer(void *ptr)					/* consumer data */
{
	int i;

	for(i = 1; i<=MAX; i++)
	{
		pthread_mutex_lock(&the_mutex);		/* mutex buffer */
		while(buffer == 0) pthread_cond_wait(&condc, &the_mutex);
		buffer = 0;							/* grab the data */
		pthread_cond_signal(&condp);		/* wake the producer */
		pthread_mutex_unlock(&the_mutex);	/* release the buffer */
	}
	pthread_exit(0);
}

int main(int argc, char **argv)
{
	pthread_t pro, con;
	pthread_mutex_init(&the_mutex,0);
	pthread_cond_init(&condc,0);
	pthread_cond_init(&condp,0);
	pthread_create(&con,0,consumer,0);
	pthread_create(&pro,0,producer,0);
	pthread_join(pro,0);
	pthread_join(con,0);
	pthread_cond_destroy(&condc);
	pthread_cond_destroy(&condp);
	pthread_mutex_destroy(&the_mutex);

	return 0;
}
