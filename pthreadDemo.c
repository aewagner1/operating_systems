#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int numThreads;
void* test(void* r);

int main(int c, char* argv[]) {
    // Read the commandline arguments
    numThreads = atoi(argv[1]);	/* Where to start */
    int limit = atoi(argv[2]);
    printf("N = %d, qs = %d\n", numThreads, limit);

    // Launch the threads
    pthread_t* thread_handles = (pthread_t*)malloc(numThreads * sizeof(pthread_t));

    long i;
    for (i = 0; i < numThreads; i++) {
        pthread_create(&thread_handles[i], NULL, test, (void*)i);
    }
    printf("Main Thread\n");

    for (i = 0; i < numThreads; i++) {
        pthread_join(thread_handles[i], NULL);
    }
}

void* test(void* r) {
    long rank = (long)r;

    printf("Hi from thread %ld of %d\n", rank, numThreads);

    return 0;
}