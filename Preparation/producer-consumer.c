#define N 100			// buffer slots
int count = 0;			// buffer data amount

void produer(void)
{
	int item;

	while(TRUE)
	{
		item = produce_item();				// new data item
		if(count == N) sleep();				// if filled, sleep
		insert_item(item);					// put new
		count += 1;							// add one
		if(count == 1) wakeup(consumer);
	}
}

void consumer(void)
{
	int item;

	while(TRUE)
	{
		if(count == 0) sleep();
		item = remove_item();
		count--;
		if(count == N-1) wakeup(producer);
		consume_item(item);
	}
}
