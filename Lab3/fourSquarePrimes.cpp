#include <stdio.h>
#include <iostream>
#include <math.h>
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
  mutex qmutex;   /* protect count        */
  int *items;     /* array of queue ints  */
};
typedef struct queue queue;

/*
 * Forward declarations of some functions
 */
int minNumSquares(int N);
bool isPrime(int N);
void addInts(int N);
void findPrimes(int N);
void findFourSquares(int N);
void addItem(queue *q, int x);
int getItem(queue *q);

/*
 * Global variables
 */
int queueSize;
int numThreads = 1000;
queue queue0, queue1, queue2;


/*
 * Prints out all prime numbers within a range of 1000 numbers
 *   that require four squares. The user inputs a number
 *   representing "where to start," and the program checks the
 *   block of 10000 integers starting there.
 *
 * Two commandline parameters:
 *   1:  Where to start
 *   2:  Size of queues
 */
int main(int c, char *argv[]) {
  // Read the commandline arguments
  int N = atoi(argv[1]); /* Where to start */
  queueSize = atoi(argv[2]);
  cout << "N = " << N << ", queueSize = " << queueSize << endl;

  // Allocate memory for the items arrays within the queues
  // Your work here...
  queue0.items = new int[queueSize];
  queue1.items = new int[queueSize];
  queue2.items = new int[queueSize];
  cout << "Allocated int arrays in the queues" << endl;

  // Initialize the counts and locOfFirsts of the queues
  queue0.count = queue0.locOfFirst = 0;
  queue1.count = queue1.locOfFirst = 0;
  queue2.count = queue2.locOfFirst = 0;
  cout << "Initialized count and loc" << endl;

  // Initialize the queue semaphores
  sem_init(&queue0.spaceSem, 0, queueSize);
  sem_init(&queue0.itemsSem, 0, 0);
  sem_init(&queue1.spaceSem, 0, queueSize);
  sem_init(&queue1.itemsSem, 0, 0);
  sem_init(&queue2.spaceSem, 0, queueSize);
  sem_init(&queue2.itemsSem, 0, 0);  

  // Launch the threads
  thread intsT, primesT, foursT[numThreads]; // Make foursT into an array of threads
  intsT = thread(addInts, N);
  primesT = thread(findPrimes, 0);
  for (int i = 0; i < numThreads; i++) {
    foursT[i] = thread(findFourSquares, i); // Change to array
  }
  cout << "Launched Threads" << endl;

  // Grab items off of the last queue and print them.
  while (1) {
    int x = getItem(&queue2);
    if (x == -1)
      break;
    printf("%d is prime, and needs 4 squares\n", x);
  }

  // Join the child threads
  intsT.join();
  primesT.join();
  for (int i = 0; i < numThreads; i++) {
    foursT[i].join();
  }

  return 0;
}

/*
 * Thread to simply add items from N to N+999 to queue0
 *
 * Adds -1 token to queue when done.
 */
void addInts(int startValue) {
  int i;
  for (i = startValue; i < startValue + 1000; i++) {
    addItem(&queue0, i);
  }
  addItem(&queue0, -1);
  cout << "addInts done" << endl;
}

/*
 * Thread to take items from the queue0, and place those
 * that are prime onto queue1.
 *
 * Stops when it sees a -1 token. Adds -1 token to queue when done.
 */
void findPrimes(int dummy) {
  while (1) {
      int i = getItem(&queue0);
      if (isPrime(i))
        addItem(&queue1, i);
      if (i == -1) {
          addItem(&queue1, -1);
          break;
      }
  }
  printf("findPrimes done\n");
}

/*
 * Thread to take primes off of queue1, and place those
 * that require four squares onto queue2
 *
 * Stops when it sees a -1 token. Adds -1 token to queue when done.
 */
void findFourSquares(int dummy) {
  while (1) {
      int i = getItem(&queue1);
      if (minNumSquares(i) == 4)
        addItem(&queue2, i);
      else if (i == -1) {
          addItem(&queue2, -1);
          addItem(&queue1, -1);
          break;
      }
  }
  printf("findFourSquares done\n");
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
 * Before getting an item, it waits on the items semaphore to make
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
 * Returns the minimum number of square numbers (including 0)
 * needed to represent the input number N as the sum of those squares.
 *
 * By Lagrange's four squares theorem, the answer will always be <= 4.
 */
int minNumSquares(int N) {
  int sq = sqrt(N);
  int a, b, c, d; /* counters */
  int numNotZero = 0;

  for (a = 0; a <= sq; a++) {
    for (b = a; b <= sq; b++) {
      for (c = b; c <= sq; c++) {
        for (d = c; d <= sq; d++) {
          if (N == a * a + b * b + c * c + d * d)
            goto found;
        }
      }
    }
  }
found:
  if (a > 0)
    numNotZero++;
  if (b > 0)
    numNotZero++;
  if (c > 0)
    numNotZero++;
  if (d > 0)
    numNotZero++;

  return numNotZero;
}

/*
 * Returns true if the number is a prime >= 2
 * and false otherwise.
 */
bool isPrime(int N) {
  if (N < 2 || (N % 2) == 0)
    return false;
  if (N == 2)
    return true;

  int i; /* counter */
  for (i = 3; i <= sqrt(N); i = i + 2) {
    if (N % i == 0)
      return false;
  }

  return true; /* If we make it here, N has no divisors */
}