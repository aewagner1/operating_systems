#include <iostream>
#include <mutex>
#include <semaphore.h>
#include <thread>

using namespace std;

/*
 * Queue structure
 */
struct queue {
  int count;      /* # items in the queue */
  int locOfFirst; /* index of first item  */
  sem_t spaceSem; /* # remaining spaces   */
  sem_t itemsSem; /* # items on queue     */
  mutex qmutex;    /* protect count        */
  int *items;     /* array of queue ints  */
};
typedef struct queue queue;

/*
 * Forward declarations of some functions
 */
void addItem(queue *q, int x);
int getItem(queue *q);
void producer(int N);
void consumer(int N);

/*
 * Global variables
 */
int queueSize;
int N; // Number of items to produce
queue queue0;

/*
 * Command line parameter: Size of queue
 */
int main(int c, char *argv[]) {
  // Read the commandline arguments
  queueSize = atoi(argv[1]); // Size of queue
  int N = atoi(argv[2]);    // Number of items to make and enqueue
  cout << "Putting " << N << " items on queue of size " << queueSize << endl;

  // Allocate memory for the items arrays within the queues
  // Your work here...
  queue0.items = new int[queueSize];
  printf("Allocated memory\n");

  // Initialize the counts and locOfFirsts of the queues
  queue0.count = 0;
  queue0.locOfFirst = 0;
  printf("Initialized count and loc\n");

  // Initialize the queue semaphores
  sem_init(&queue0.itemsSem, 0, 0);
  sem_init(&queue0.spaceSem, 0, queueSize);
  printf("Initialized semaphores\n");

  // Launch the threads
  thread maker_thread = thread(producer, N);
  thread consumer_thread = thread(consumer, 0);
  printf("Launched Threads\n");

  // Join the child threads
  maker_thread.join();
  consumer_thread.join();

  return 0;
}

/*
 * Thread to make and enqueue items
 */
void producer(int N) {
  for (int i = 0; i < N; i++) {
    addItem(&queue0, i);
  }
  addItem(&queue0, -1);
  printf("addInts done\n");
}

/*
 * Thread to take items from the queue0, and place those
 * that are prime onto queue1.
 */
void consumer(int N) {
  int x;
  do {
    x = getItem(&queue0);
    cout << x << ".";
  } while (x != -1);
  printf("\nconsumer done\n");
}

/*
 * This function adds an item x to the given queue, q.
 *
 * It uses the queue's mutex to protect access to the count
 *   and to the items in the queue.
 *
 * Before taking an item, it waits on the space semaphore to make
 *   sure there is room to add the item. And then it posts to the
 *   items semaphore to announce that there is another in the queue.
 */
void addItem(queue *q, int x) {
  sem_wait(&q->spaceSem);
  q->qmutex.lock();
  int locOfNext = (q->locOfFirst + q->count) % queueSize;
  q->items[locOfNext] = x;
  q->count++;
  q->qmutex.unlock();
  sem_post(&q->itemsSem);
}

/*
 * This function removes and returns an item from the given queue, q.
 *
 * It uses the queue's mutex to protect access to the count and locOfFirst,
 *   and to the items in the queue.
 *
 * Before adding an item, it waits on the items semaphore to make
 *   sure there is an item to return. And then it posts to the
 *   spaces semaphore to announce that there is another open space.
 */
int getItem(queue *q) {
  sem_wait(&q->itemsSem);
  q->qmutex.lock();
  int itemToReturn = q->items[q->locOfFirst];
  q->count--;
  q->locOfFirst = (q->locOfFirst + 1) % queueSize;
  q->qmutex.unlock();
  sem_post(&q->spaceSem);
  return itemToReturn;
}

/*
 * Visualization of the queues
 */
void printQueues() {
  for (int j = 0; j < queueSize; j++) {
    int d1 = queue0.locOfFirst - j;
    int locOfLast =
        (queue0.locOfFirst + queue0.count + queueSize - 1) % queueSize;
    int d2 = locOfLast - j;
    int d3 = locOfLast - queue0.locOfFirst;
    if (d1 * d2 * d3 <= 0)
      printf("*");
    else
      printf(".");
  }
}