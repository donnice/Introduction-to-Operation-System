#define N 100		/* slot of buffer */
// page 96

void producer(void)
{
	int item;
	message m;		/* message buffer */

	while(TRUE)
	{
		item = produce_item();		/* generate data in buffer */
		receive(consumer,&m);		/* wait consumer send emp buf */
		build_message(&m,item);		/* build a message to be sent */
		send(consumer,&m);			/* send to consumer */
	}
}

void consumer(void)
{
	int item,i;
	message m;

	for(i = 0; i < N; i++) send(producer,&m);
	while(TRUE)
	{
		receive(producer,&m);		/* receive message */
		item = extract_item(&m);	/* extract message */
		send(producer,&m);			/* send empty buffer producer */
		consume_item(item);			/* consume data */
	}
}
