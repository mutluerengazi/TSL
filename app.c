#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "tsl.h"
#define MAXCOUNT 5
#define YIELDPERIOD 100


void foo(void *v) {  // Changed from `void *foo(void *v)` to `void foo(void *v)`
    int count = 1;
    int mytid;

    mytid = tsl_gettid();
    printf("foo\n");
    printf("thread %d started running (first time); at the start function\n", mytid);

    while (1) {
        printf("thread %d is running (count=%d)\n", mytid, count);
        if (count % YIELDPERIOD == 0) {
            printf("BABAN ve ANNEN");
        }
        count++;
        if (count == MAXCOUNT)
            break;
    }
    // Since the function now returns void, we remove the return statement.
}




int main(int argc, char *argv[]) {
    int *tids;
    int numthreads = 0;

    if (argc != 2) {
        printf("usage: ./app numofthreads\n");
        exit(1); 
    }

    numthreads = atoi(argv[1]);

    tids = (int *) malloc(numthreads * sizeof(int));
    if (!tids) {
        perror("Failed to allocate memory for tids");
        exit(1);
    }

    // Assuming ALG_FCFS is defined elsewhere
    if (tsl_init(ALG_FCFS) != 0) {
        fprintf(stderr, "Failed to initialize threading library.\n");
        free(tids);
        exit(1);
    }
    tids[0] = 1; // Assuming the main thread ID is 1 as set by tsl_init
    tids[1] = tsl_create_thread((void (*)(void *))foo, NULL);
    //tids[2] = tsl_create_thread((void (*)(void *))foo, NULL);
    printf("tsl yield result: %d \n", tsl_yield(2));


    printf("main thread calling tsl_exit\n");
    tsl_exit();

    free(tids); // Don't forget to free the allocated memory
    return 0;
}
