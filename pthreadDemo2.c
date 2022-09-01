#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int numThreads;
long counter;
void* test(void* r);

// Multiplies two commandline arguments a and b, 
// Launches a threads, each of which adds b to the counter.
int main(int c, char* argv[]) {
    // Read the commandline arguments
    numThreads = atoi(argv[1]);	/* Where to start */
    long factor = atoi(argv[2]);
    printf("N = %d, qs = %ld\n", numThreads, factor);

    // Initialize Counter
    counter = 0;

    // Launch the threads
    pthread_t* thread_handles = (pthread_t*)malloc(numThreads * sizeof(pthread_t));

    long i;
    for (i = 0; i < numThreads; i++) {
        pthread_create(&thread_handles[i], NULL, test, (void*)factor);
    }

    for (i = 0; i < numThreads; i++) {
        pthread_join(thread_handles[i], NULL);
    }

    printf("The product of %d and %ld is %ld\n", numThreads, factor, counter);
}

void* test(void* f) {
    long factor = (long)f;

    for(int i = 0; i < factor; i++){
        counter++;
    }

    return 0;
}