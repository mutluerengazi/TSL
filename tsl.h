#ifndef _TSL_H_
#define _TSL_H_

#include <ucontext.h>

#define TSL_MAXTHREADS 258
#define TSL_STACKSIZE 32768

#define ALG_FCFS 1
#define ALG_RANDOM 2

#define TID_MAIN 1 // tid of the main tread this id is reserved for main thread.
#define TSL_ANY 0 // yield to a thread selected with a scheduling alg.
#define TSL_ERROR -1  // there is an error in the function execution.
#define TSL_SUCCESS 0 // function execution success

typedef enum {
  TSL_RUNNING, 
  READY,     
  BLOCKED,     
  ENDED,
} ThreadState;

typedef struct TCB {
  int tid;            
  int state;          
  ucontext_t context; 
  char *stack;       
} TCB;

typedef struct tsl_library_state {
  int scheduling_algorithm; 
  TCB *main_thread_tcb;    
  TCB *current_thread;     
  int num_threads; 
} TSL_Library_State;

typedef struct TCBNode {
  struct TCB *tcb;
  struct TCBNode *next;
} TCBNode;

typedef struct queue {
  struct TCBNode *head;
  int numOfThreads;
} queue;

int tsl_init(int salg);
int tsl_create_thread(void (*tsf)(void *), void *targ);
int tsl_yield(int tid);
int tsl_exit();
int tsl_join(int tid);
int tsl_cancel(int tid);
int tsl_gettid();

#endif
