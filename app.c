#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "tsl.h"
#define MAXCOUNT 1000
#define YIELDPERIOD 100

void *foo(void *v)
{
	int count = 1;
    int mytid;
    
    mytid = tsl_gettid();
    printf ("thread %d started running (first time);  at the start function\n", mytid);

	//while (count < MAXCOUNT) {
    while (1) {
        printf ("thread %d is running (count=%d)\n", mytid, count);
		if (count % YIELDPERIOD == 0) {
			tsl_yield (TSL_ANY);
		}
        count++;
        if (count == MAXCOUNT)
                break;
	}
	return (NULL); 
}


int main(int argc, char *argv[]) {
    int *tids;
    int i;
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

    for (i = 1; i < numthreads; ++i) {
        tids[i] = tsl_create_thread(foo, NULL);
        if (tids[i] == TSL_ERROR) {
            fprintf(stderr, "Failed to create thread.\n");
            // Handle error as appropriate, perhaps by cleaning up and exiting
        } else {
            printf("Thread %d created\n", tids[i]);
        }
    }

    for (i = 1; i < numthreads; ++i) {
        printf("main: waiting for thread %d\n", tids[i]);
        tsl_join(tids[i]);
        printf("main: thread %d finished\n", tids[i]);
    }

    printf("main thread calling tsl_exit\n");
    tsl_exit();

    free(tids); // Don't forget to free the allocated memory
    return 0;
}
