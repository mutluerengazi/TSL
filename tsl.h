#ifndef _TSL_H_
#define _TSL_H_

// do not change this header file (tsl.h) 
// it is the interface of the tsl library to the applications

#include <ucontext.h>


#define TSL_MAXTHREADS 258
#define TSL_STACKSIZE  32768

#define ALG_FCFS 1
#define ALG_RANDOM 2
#define ALG_MYALGORITHM 3

#define TID_MAIN 1 // tid of the main tread. this id is reserved for main thread.
#define TSL_ANY 0  // yield to a thread selected with a scheduling alg.
#define TSL_ERROR  -1  // there is an error in the function execution.
#define TSL_SUCCESS 0  // function execution success

typedef enum {
    TSL_RUNNING, // Running state
    READY,   // Ready state
    BLOCKED, // Blocked state
} ThreadState;

typedef struct TCB {
    int tid; // thread identifier
    unsigned int state; // thread state
    ucontext_t context; // pointer to context structure
    char *stack; // pointer to stack
} TCB;

typedef struct tsl_library_state {
    int scheduling_algorithm; // The scheduling algorithm being used
    TCB *main_thread_tcb;     // TCB for the main thread
    TCB *current_thread;      // TCB for the current thread
    int num_threads;          // The number of threads currently managed by the library
    // Add structures for managing TCBs and the ready queue here
} TSL_Library_State;



int tsl_init(int salg);
int tsl_create_thread (void (*tsf)(void *), void *targ);
int tsl_yield (int tid);
int tsl_exit();
int tsl_join(int tid);
int tsl_cancel(int tid);
int tsl_gettid();

#endif