#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <assert.h>

/* We want the extra information from these definitions */
#ifndef __USE_GNU
#define __USE_GNU
#endif /* __USE_GNU */

#include <ucontext.h>
#include "tsl.h"
// Define constants for scheduling algorithms if not already defined
#ifndef SCHED_ALG_1
#define SCHED_ALG_1 1
#endif

// Define TSL ERROR if not already defined
#ifndef TSL_ERROR
#define TSL_ERROR -1
#endif


int tsl_init(int salg);
int tsl_create_thread (void (*tsf)(void *), void *targ);
int tsl_yield (int tid);
int tsl_exit();
int tsl_join(int tid);
int tsl_cancel(int tid);
int tsl_gettid();

struct queue *ReadyQueue;
struct queue *WaitQueue;

TSL_Library_State *library_state = NULL;

static void thread_stub(void (*tsf)(void *), void *targ);
int generate_tid();

int tid_assign = 1;

int  tsl_init(int salg) {
    // Ensure the library is not already initialized
    if (library_state != NULL) {
        fprintf(stderr, "Library is already initialized.\n");
        return TSL_ERROR;
    }

    // Allocate memory for the library state
    library_state = (TSL_Library_State *)malloc(sizeof(TSL_Library_State));
    if (library_state == NULL) {
        fprintf(stderr, "Failed to allocate memory for library state.\n");
        return TSL_ERROR;
    }

    // Set the scheduling algorithm and initialize num_threads
    library_state->scheduling_algorithm = salg;
    library_state->num_threads = 1; // Starting with the main thread

    // Allocate and initialize the TCB for the main thread
    library_state->main_thread_tcb = (TCB *)malloc(sizeof(TCB));
    if (library_state->main_thread_tcb == NULL) {
        fprintf(stderr, "Failed to allocate TCB for the main thread.\n");
        free(library_state);
        library_state = NULL;
        return TSL_ERROR;
    }

    // Initialize the main thread TCB, e.g., getting its context, setting its state and ID
    // This is simplified here and should be expanded according to your project requirements
    // library_state->main_thread_tcb->tid = 1; // Assigning a unique identifier to the main thread
    // library_state->main_thread_tcb->state = TSL_RUNNING; // Assuming RUNNING is a defined state
    // Initialize the main thread TCB, e.g., getting its context, setting its state and ID
    // You should uncomment or complete these lines based on your implementation details
    library_state->main_thread_tcb->tid = TID_MAIN; // Assigning a unique identifier to the main thread
    library_state->main_thread_tcb->state = TSL_RUNNING; // Assuming RUNNING is a defined state

    // Set the current thread to the main thread's TCB
    library_state->current_thread = library_state->main_thread_tcb;

    // If necessary, initialize your ready queue and other data structures here
    ReadyQueue = (struct queue *) malloc(sizeof (struct queue));
    ReadyQueue->head = (struct TCBNode *) malloc(sizeof (struct TCBNode));
    ReadyQueue->head->next = NULL;
    ReadyQueue->head->tcb = (struct TCB *) malloc(sizeof (struct TCB));
    ReadyQueue->head->tcb->tid = TID_MAIN;
    ReadyQueue->head->tcb->state = READY;
    ReadyQueue->numOfThreads = 1;

    return 0; // Success
}



int tsl_create_thread(void (*tsf)(void *), void *targ) {
    if (!library_state || library_state->num_threads >= TSL_MAXTHREADS - 1) {
        fprintf(stderr, "Library is not initialized or max thread count exceeded.\n");
        return TSL_ERROR;
    }

    TCB *new_thread_tcb = (TCB *)malloc(sizeof(TCB));
    if (new_thread_tcb == NULL) {
        fprintf(stderr, "Failed to allocate memory for new thread TCB.\n");
        return TSL_ERROR;
    }

    // Initialize the context for the new thread
    if (getcontext(&new_thread_tcb->context) == -1) {
        free(new_thread_tcb);
        fprintf(stderr, "Failed to get context for new thread.\n");
        return TSL_ERROR;
    }

    new_thread_tcb->stack = (char *)malloc(TSL_STACKSIZE);
    if (new_thread_tcb->stack == NULL) {
        free(new_thread_tcb);
        fprintf(stderr, "Failed to allocate stack for new thread.\n");
        return TSL_ERROR;
    }

    new_thread_tcb->context.uc_stack.ss_sp = new_thread_tcb->stack;
    new_thread_tcb->context.uc_stack.ss_size = TSL_STACKSIZE;
    new_thread_tcb->context.uc_link = 0; // No successor context

    // Adjusting for x86 architecture specifics
    new_thread_tcb->context.uc_mcontext.gregs[REG_EIP] = (unsigned long)thread_stub;
    new_thread_tcb->context.uc_mcontext.gregs[REG_ESP] = (unsigned long)(new_thread_tcb->stack + TSL_STACKSIZE - sizeof(void *) * 2); // Stack grows downward

    // Place `tsf` and `targ` on the new thread's stack for `thread_stub` to use
    void **stack_top = (void **)(new_thread_tcb->context.uc_mcontext.gregs[REG_ESP]);
    stack_top[0] = targ;
    stack_top[1] = tsf;

    new_thread_tcb->tid = generate_tid(); // Assuming generate_tid() generates a unique ID
    new_thread_tcb->state = READY;

    // Add new_thread_tcb to your scheduler's ready queue here
    TCBNode *new_tcb_node = (struct TCBNode *) malloc(sizeof (struct TCBNode));
    new_tcb_node->tcb = new_thread_tcb;

    TCBNode *last = ReadyQueue->head;
    while (last->next){
        last = last->next;
    }
    last->next = new_tcb_node;
    ReadyQueue->numOfThreads = ReadyQueue->numOfThreads + 1;

    library_state->num_threads++; // Assuming you're keeping track of thread count
    library_state->current_thread->tid = new_thread_tcb->tid;
    return new_thread_tcb->tid;
}


static void thread_stub(void (*tsf)(void *), void *targ) {
    tsf(targ); // Call the thread start function with the argument
    tsl_exit(); // Terminate the thread when the start function returns
}

int generate_tid() {
    tid_assign++;
    return tid_assign;
}


int tsl_yield(int tid)
{
    printf("SELAM\n");
    int context_flag = 0;
    
    if(tid == TSL_ANY){
        //schedule algorithm
    }// if current tid = tid
    else if(tid ==  ReadyQueue->head->tcb->tid){
        printf("KENDIMDEYIM\n");
        printf("READY QUEUE: %d\n", ReadyQueue->head->tcb->tid);
        getcontext(&(ReadyQueue->head->tcb->context));
        if (context_flag == 0) {
            context_flag = 1;
            setcontext(&(ReadyQueue->head->tcb->context));
        }
        else{
            context_flag = 0;
            return ReadyQueue->head->tcb->tid;
        }
    }
    else
    {
        printf("SELAM123123\n");
        //adding current thread to ready queue
        printf("READY QUEUE: %d \n", ReadyQueue->head->tcb->tid);
        printf("SELAM123123\n");
        if (ReadyQueue->head == NULL)
        {
            printf("Queue empty... \n");
            return TSL_ERROR;
        }
        printf("SELAM123123\n");
        // current thread is at the ready que
        TCBNode *prev_node = NULL;
        TCBNode *node_to_yield = ReadyQueue->head;

        if (node_to_yield->tcb->tid == tid)
        {
            getcontext(&(node_to_yield->tcb->context));
            if (context_flag == 0)
            {
                context_flag = 1; 
                setcontext(&(node_to_yield->tcb->context));
            }
            else
            {
                context_flag = 0;
                return ReadyQueue->head->tcb->tid;
            }
        }
        else
        {
            int return_id = ReadyQueue->head->tcb->tid;
            getcontext(&(ReadyQueue->head->tcb->context));
            if(context_flag == 0){
                while (node_to_yield){
                    if (node_to_yield->tcb->tid == tid){
                        break;
                    }
                    prev_node = node_to_yield;
                    node_to_yield = node_to_yield->next;
                }
                if (node_to_yield)
                {
                    
                    printf("node to yield tid1: %d \n", node_to_yield->tcb->tid);
                    prev_node->next = node_to_yield->next;
                    node_to_yield->next = ReadyQueue->head;
                    ReadyQueue->head = node_to_yield;
                
                    ReadyQueue->head->next->tcb->state = READY;
                    ReadyQueue->head->tcb->state = TSL_RUNNING;

                   
                    
                    //getcontext(&(node_to_yield->tcb->context));
                    printf("node to yield tid2: %d \n", node_to_yield->tcb->tid);

            
                    printf("anan12\n");
                    context_flag = 1;
                    //printf("ready queue context %d\n", ReadyQueue->head->tcb->context);
                    setcontext(&(node_to_yield->tcb->context));
                
                }
            }
            else{
                context_flag = 0;
                printf("anan123\n");
                return return_id;
            }
            printf("SELAM123123\n");

        }
        
    }
    return (0);
}


int tsl_exit()
{
    return (0);
}

int tsl_join(int tid)
{
    return (0);
}


int tsl_cancel(int tid)
{

    return (0);
}

int tsl_gettid() {
    if (library_state != NULL && library_state->current_thread != NULL) {
        return library_state->current_thread->tid;
    } else {
        // This may indicate that the library is not initialized, or no thread is running.
        return TSL_ERROR;
    }
}
