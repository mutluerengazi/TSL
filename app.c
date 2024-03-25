#include "tsl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXCOUNT 5
#define YIELDPERIOD 100

void foo(void *v) { 
  int count = 1;
  int mytid;

  mytid = tsl_gettid();
  printf("foo(tsf) starts here...\n");
  printf("thread %d started running (first time); at the start function\n",mytid);
  
  while (1) {
    printf("thread %d is running (count=%d)\n", mytid, count);
    count++;
    if (count == MAXCOUNT)
      break;
  }
  
  printf("tsl yield result 3: %d \n", tsl_yield(3));
  printf("tsl yield result 3: %d \n", tsl_yield(4));
  exit(0);
}

int main(int argc, char *argv[]) {
  int *tids;
  int numthreads = 0;

  if (argc != 2) {
    printf("usage: ./app numofthreads\n");
    exit(1);
  }

  numthreads = atoi(argv[1]);

  tids = (int *)malloc(numthreads * sizeof(int));
  if (!tids) {
    perror("Failed to allocate memory for tids");
    exit(1);
  }

  // algorithm defined below ALG_FCFS or ALG_RANDOM
  if (tsl_init(ALG_FCFS) != 0) {
    fprintf(stderr, "Failed to initialize threading library.\n");
    free(tids);
    exit(1);
  }
  tids[0] = 1; // main thread
  tids[1] = tsl_create_thread((void (*)(void *))foo, NULL);
  tids[2] = tsl_create_thread((void (*)(void *))foo, NULL);
  tids[3] = tsl_create_thread((void (*)(void *))foo, NULL);
  tids[4] = tsl_create_thread((void (*)(void *))foo, NULL);
  tids[5] = tsl_create_thread((void (*)(void *))foo, NULL);
  tids[6] = tsl_create_thread((void (*)(void *))foo, NULL);
  //tsl_cancel(4);
  tsl_join(2); 
  printf("main thread calling exit\n");

  free(tids);
  return 0;
}
