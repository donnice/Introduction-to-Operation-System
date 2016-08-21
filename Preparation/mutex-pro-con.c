#define N 100
typedef int semaphore;
semaphore mutex = 1;
semaphore empty = N;
semaphore full = 0;

void producer(void)
{
	int item;

	while(TRUE)
	{
		item = produce_item();
		down(&empty);		// empty - 1
		down(&mutex);		// come inter field
		insert_item(item);	// put in buffer
		up(&mutex);			// leave inter field
		up(&full);			// add item number
	}
}

void consumer(void)
{
	int item;

	while(TRUE)
	{
		down(&full);			// minus 1
		down(&mutex);			// come inter field
		item = remove_item();	// take item
		up(&mutex);				// leave inter field
		up(&empty);				// add 1 empty
		consume_item(item);
	}
}
