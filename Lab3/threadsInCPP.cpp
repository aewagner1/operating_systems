#include <iostream>       
#include <thread>         

using namespace std;

int numThreads;
void test(int r);

int main(int c, char *argv[]) {
  // Read the commandline arguments
  numThreads = atoi(argv[1]); /* Where to start */ 
  cout << "N = " << numThreads << endl;

  // Array to hold the thread objects
  thread threads[numThreads];

  // Launch the threads
  int i;
  for (i = 0; i < numThreads; i++) {
    threads[i] = thread(test, i);
  }

  printf("Main Thread\n");

  // Wait for the threads to finish
  for (i = 0; i < numThreads; i++) {
    threads[i].join();
  }
}

// This is the "main" function for the threads
void test(int rank) {
  printf("Hi from thread %d of %d\n", rank, numThreads);
}
