#include <iostream>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include <time.h>

using namespace std;

// Readers and Writers Data
int numReaders, numWriters;
void reader(int rank);
void writer(int rank);
mutex count_mutex;
int active_readers;
sem_t data_sem;

// The data structure they read and write
int myMoney[11]; // myMoney[10] = sum of myMoney[0-9]

// Helper function to print balances
void printMoney(){
    for(int i = 0; i < 11; i++){
        cout << myMoney[i] << " ";
    }
    cout << endl;
}

int main(int c, char *argv[]) {
  // Read the commandline arguments
  numReaders = atoi(argv[1]);
  numWriters = atoi(argv[2]);
  cout << numReaders << " readers, " << numWriters << " writers." << endl;

  // Array to hold the thread objects
  thread readers[numReaders];
  thread writers[numWriters];

  // Synchronization variables
  active_readers = 0;
  sem_init(&data_sem, 0, 1); // Initialize to 1

  // Create and launch the readers and writers
  srand(time(NULL)); // seed the random number generator
  int i;
  for (i = 0; i < numReaders; i++) {
    readers[i] = thread(reader, i);
  }

  for (i = 0; i < numWriters; i++) {
    writers[i] = thread(writer, i);
  }

  printf("Main Thread\n");

  // Wait for the threads to finish
  for (i = 0; i < numReaders; i++) {
    readers[i].join();
    // cout << "Reader #" << i << " done." << endl;
  }

  for (i = 0; i < numWriters; i++) {
    writers[i].join();
    // cout << "Writer #" << i << " done." << endl;
  }

  cout << "Final Accounting" << endl;
  printMoney();
}

// This is the "main" function for the reader threads
// Each reader checks for consistency 100 times
void reader(int rank) {
  for (int i = 0; i < 140; i++) {
    // Protection to enter
    count_mutex.lock();
    active_readers++;
    if (active_readers == 1)
      sem_wait(&data_sem);
    count_mutex.unlock();

    // Check consistency
    int sum = 0;
    for (int i = 0; i < 10; i++)
      sum += myMoney[i];
    if (sum != myMoney[10]){
      cout << "Violation!!! by reader " << rank << endl;
    } else {
        printMoney();
    }

    // Leave the data structure
    count_mutex.lock();
    active_readers--;
    if (active_readers == 0)
      sem_post(&data_sem);
    count_mutex.unlock();

    int sleeptime = rand() % 20; // sleep for up to 20ms
    this_thread::sleep_for(chrono::milliseconds(sleeptime));
  }
}

// This is the "main" function for the writer threads
// Each writer adds money 9 times
void writer(int rank) {
  // Do the work
  for (int i = 0; i < 9; i++) {
    // Permission to enter
    sem_wait(&data_sem);
    int sum = 0;
    for (int i = 0; i < 10; i++) {
      int deposit = rand() % 10;
      myMoney[i] += deposit;
      sum += deposit;
      this_thread::sleep_for(chrono::milliseconds(rand() % 10));
    }
    myMoney[10] += sum;

    // Release the semaphore
    sem_post(&data_sem);

    int sleeptime = rand() % 1000;
    this_thread::sleep_for(chrono::milliseconds(sleeptime));
  }
}