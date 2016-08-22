public class ProducerConsumer {
	static final int N = 100;		// buffer
	static producer p = new producer();		// producer thread
	static consumer c = new consymer();		// consumer thread
	static our_monitor mon = new our_monitor();	// new pipe process

	public static void main(String[] args) {
		p.start();		// producer start
		c.start();		// consumer start
	}

	static class producer extends Thread {
		public void run() {
			int item;
			while(true){
				item = produce_item();
				mon.insert(item);		// generate loop
			}
		}
		private int produce_item(){
			System.out.println("Producing one item!");
			return 1;
		}
	}

	static class consumer extends Thread {
		public void run(){
			int item;
			while(true) {
				item = mon.remove();
				consume_item(item);
			}
		}
		private void consume_item(int item){
			System.out.println("I am consuming it!"):
		}
	}

	static class our_monitor {
		private int buffer[] = new int[N];
		private int count = 0, lo = 0, hi = 0;

		public synchronized void insert(int val){
			if(count == N) go_to_sleep();		// if full, go sleep
			buffer[hi] = val;	// insert new into buffer
			hi = (hi+1)%N;		// set next data slot
			count++;
			if(count == 1) notify();	// if consumer sleep, wake him up
		}

		public synchronized int remove() {
			int val;
			if(count == 0) go_to_sleep();
			val = buffer[lo];		// get data from buffer
			lo = (lo+1)%N;			// set a slot for data
			count--;
			if(count == N-1) notify();	// if producer sleep, wake up
			return val;
		}
		private void go_to_sleep(){
			try {
				wait();
			} catch(InterruptedException exc){};
		}

	}
