#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#ifndef __USE_GNU
#define __USE_GNU
#endif 

#include "tsl.h"
#include <stdlib.h>
#include <time.h>
#include <ucontext.h>

#ifndef SCHED_ALG_1
#define SCHED_ALG_1 1
#endif


#ifndef TSL_ERROR
#define TSL_ERROR -1
#endif

int tsl_init(int salg);

int tsl_create_thread(void (*tsf)(void *), void *targ);

int tsl_yield(int tid);

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

int tsl_init(int salg) {
    if (library_state != NULL) {
        fprintf(stderr, "Library is already initialized.\n");
        return TSL_ERROR;
    }
    printf("salg %d \n", salg);
    // allocate memory for the library state
    library_state = (TSL_Library_State *) malloc(sizeof(TSL_Library_State));

    if (library_state == NULL) {
        fprintf(stderr, "Failed to allocate memory for library state.\n");
        return TSL_ERROR;
    }

    // set the scheduling algorithm and initialize num_threads
    library_state->scheduling_algorithm = salg;
    library_state->num_threads = 1; // Starting with the main thread

    // allocate and initialize the TCB for the main thread
    library_state->main_thread_tcb = (TCB *) malloc(sizeof(TCB));
    if (library_state->main_thread_tcb == NULL) {
        fprintf(stderr, "Failed to allocate TCB for the main thread.\n");
        free(library_state);
        library_state = NULL;
        return TSL_ERROR;
    }

    library_state->main_thread_tcb->tid = TID_MAIN;
    library_state->main_thread_tcb->state = TSL_RUNNING;

   
    library_state->current_thread = library_state->main_thread_tcb;

    
    ReadyQueue = (struct queue *) malloc(sizeof(struct queue));
    ReadyQueue->head = (struct TCBNode *) malloc(sizeof(struct TCBNode));
    ReadyQueue->head->next = NULL;
    ReadyQueue->head->tcb = (struct TCB *) malloc(sizeof(struct TCB));
    ReadyQueue->head->tcb->tid = TID_MAIN;
    ReadyQueue->head->tcb->state = READY;
    ReadyQueue->numOfThreads = 1;

    return 0; 
}

int tsl_create_thread(void (*tsf)(void *), void *targ) {
    if (!library_state || library_state->num_threads >= TSL_MAXTHREADS - 1) {
        fprintf(stderr,
                "Library is not initialized or max thread count exceeded.\n");
        return TSL_ERROR;
    }

    TCB *new_thread_tcb = (TCB *) malloc(sizeof(TCB));
    if (new_thread_tcb == NULL) {
        fprintf(stderr, "Failed to allocate memory for new thread TCB.\n");
        return TSL_ERROR;
    }

    // initialize the context for the new thread
    if (getcontext(&new_thread_tcb->context) == -1) {
        free(new_thread_tcb);
        fprintf(stderr, "Failed to get context for new thread.\n");
        return TSL_ERROR;
    }

    new_thread_tcb->stack = (char *) malloc(TSL_STACKSIZE);
    if (new_thread_tcb->stack == NULL) {
        free(new_thread_tcb);
        fprintf(stderr, "Failed to allocate stack for new thread.\n");
        return TSL_ERROR;
    }

    new_thread_tcb->context.uc_stack.ss_sp = new_thread_tcb->stack;
    new_thread_tcb->context.uc_stack.ss_size = TSL_STACKSIZE;
    new_thread_tcb->context.uc_link = 0; 

    // adjusting for x86 architecture specifics, 32 bit
    new_thread_tcb->context.uc_mcontext.gregs[REG_EIP] = (greg_t) thread_stub;
    new_thread_tcb->context.uc_mcontext.gregs[REG_ESP] = (greg_t) (new_thread_tcb->stack + TSL_STACKSIZE - sizeof(void *) * 2); 

    // tsf and targ on the new threads stack for thread_stub to use
    void **stack_top = (void **) (new_thread_tcb->context.uc_mcontext.gregs[REG_ESP]);
    stack_top[0] = targ;
    stack_top[1] = tsf;

    new_thread_tcb->tid =
            generate_tid(); // Assuming generate_tid() generates a unique ID
    new_thread_tcb->state = READY;

    // Add new_thread_tcb to your scheduler's ready queue here
    TCBNode *new_tcb_node = (struct TCBNode *) malloc(sizeof(struct TCBNode));
    new_tcb_node->tcb = new_thread_tcb;

    TCBNode *last = ReadyQueue->head;
    while (last->next) {
        last = last->next;
    }
    last->next = new_tcb_node;
    ReadyQueue->numOfThreads = ReadyQueue->numOfThreads + 1;

    library_state->num_threads++; 
    library_state->current_thread->tid = new_thread_tcb->tid;
    return new_thread_tcb->tid;
}

static void thread_stub(void (*tsf)(void *), void *targ) {
    tsf(targ);  
    tsl_exit(); 
}

int generate_tid() {
    tid_assign++;
    return tid_assign;
}

int tsl_yield(int tid) {

    printf("*************************** tsl yield entered ***************************\n");
    int context_flag = 0;

    if (tid == TSL_ANY) {
        if (library_state->scheduling_algorithm == ALG_FCFS) {
            getcontext(&(ReadyQueue->head->tcb->context));

            if (context_flag == 0) {
                printf("FCFS scheduling...\n");
                TCBNode *curr_head = ReadyQueue->head;
                if (curr_head->next == NULL) {
                    printf("Next is empty...");
                    return TSL_ERROR;
                }
                TCBNode *prev_node = NULL;
                TCBNode *node_to_yield = curr_head->next;
                curr_head->tcb->state = READY;
                node_to_yield->tcb->state = TSL_RUNNING;
                ReadyQueue->head = node_to_yield;
                while (node_to_yield) {
                    prev_node = node_to_yield;
                    node_to_yield = node_to_yield->next;
                }
                prev_node->next = curr_head;
                curr_head->next = NULL;
                context_flag = 1;
                setcontext(&(ReadyQueue->head->tcb->context));
            } else {
                context_flag = 0;
                printf("finished...\n");
                if (ReadyQueue->head != NULL) {
                    printf("FCFS last if.. \n");
                    return ReadyQueue->head->tcb->tid;
                } else {
                    exit(0);
                }
            }
        } else if (library_state->scheduling_algorithm == ALG_RANDOM) {

            getcontext(&(ReadyQueue->head->tcb->context));

            int randomTid;
            // seed the random number generator with the current time
            srand(time(NULL));
            // generate a random number between 1 and number of threads
            randomTid = rand() % ReadyQueue->numOfThreads + 1;
            // print the random number
            printf("Random number %d\n", randomTid);
            if (context_flag == 0) {
                printf("Random scheduling...\n");
                TCBNode *curr_head = ReadyQueue->head;
                TCBNode *prev = NULL;
                TCBNode *temp = ReadyQueue->head;
                TCBNode *last = ReadyQueue->head;
                TCBNode *check_node = ReadyQueue->head;
                int check_bool = 0;
                while (check_node != NULL )
                {
                    if(check_node->tcb->tid == randomTid ){
                        check_bool = 1;
                        break;
                    }
                    check_node = check_node->next;
                }
                if (check_bool == 0)
                {
                    return TSL_ERROR;
                }
                if (check_node->tcb->state == ENDED)
                {
                    return TSL_ERROR;
                }
                
                if (curr_head->next == NULL) {
                    printf("Next is empty...");
                    return TSL_ERROR;
                }
                curr_head->tcb->state = READY;
                if (randomTid == curr_head->tcb->tid) {
                    context_flag = 1;
                    curr_head->tcb->state = TSL_RUNNING;
                    setcontext(&(ReadyQueue->head->tcb->context));
                }
                while (curr_head->tcb->tid != randomTid) {
                    prev = curr_head;
                    curr_head = curr_head->next;
                }
                if (curr_head->next == NULL) {
                    context_flag = 1;
                    ReadyQueue->head = curr_head;
                    ReadyQueue->head->next = temp->next;
                    prev->next = temp;
                    temp->next = NULL;
                    ReadyQueue->head->tcb->state = TSL_RUNNING;
                    setcontext(&(ReadyQueue->head->tcb->context));
                } else {
                    context_flag = 1;
                    prev->next = curr_head->next;
                    ReadyQueue->head = curr_head;
                    while (last->next) {
                        last = last->next;
                    }
                    curr_head->next = temp->next;
                    last->next = temp;
                    temp->next = NULL;
                    ReadyQueue->head->tcb->state = TSL_RUNNING;
                    setcontext(&(ReadyQueue->head->tcb->context));
                }
            } else {
                return ReadyQueue->head->tcb->tid;
            }
        }
    } // if current tid = tid
    else if (tid == ReadyQueue->head->tcb->tid) {
        printf("READY QUEUE tid head in itself: %d\n",ReadyQueue->head->tcb->tid);
        getcontext(&(ReadyQueue->head->tcb->context));
        if (context_flag == 0) {
            context_flag = 1;
            setcontext(&(ReadyQueue->head->tcb->context));
        } else {
            context_flag = 0;
            return ReadyQueue->head->tcb->tid;
        }
    } else {
        // adding current thread to ready queue
        printf("READY QUEUE tid head in else: %d \n", ReadyQueue->head->tcb->tid);
        if (ReadyQueue->head == NULL) {
            printf("Queue empty... \n");
            return TSL_ERROR;
        }

        // current thread is at the ready queue
        TCBNode *prev_node = NULL;
        TCBNode *node_to_yield = ReadyQueue->head;

        if (node_to_yield->tcb->tid == tid) {
            getcontext(&(node_to_yield->tcb->context));
            if (context_flag == 0) {
                context_flag = 1;
                setcontext(&(node_to_yield->tcb->context));
            } else {
                context_flag = 0;
                return ReadyQueue->head->tcb->tid;
            }
        } else {
            int return_id = ReadyQueue->head->tcb->tid;
            getcontext(&(ReadyQueue->head->tcb->context));
            if (context_flag == 0) {
                while (node_to_yield) {
                    if (node_to_yield->tcb->tid == tid) {
                        break;
                    }
                    prev_node = node_to_yield;
                    node_to_yield = node_to_yield->next;
                }
                if (node_to_yield) {
                    prev_node->next = node_to_yield->next;
                    node_to_yield->next = ReadyQueue->head;
                    ReadyQueue->head = node_to_yield;
                    printf("node to yield tid: %d \n", node_to_yield->tcb->tid);
                    if (ReadyQueue->head->tcb->state == ENDED) {
                        printf("The thread to be yielded has ended\n");
                        return TSL_ERROR;
                    }
                    ReadyQueue->head->next->tcb->state = READY;
                    ReadyQueue->head->tcb->state = TSL_RUNNING;
                    context_flag = 1;
                    setcontext(&(node_to_yield->tcb->context));
                }
            } else {
                context_flag = 0;
                return return_id;
            }
        }
    }
    return (0);
}

int tsl_exit() {
    if (ReadyQueue->head != NULL) {
        printf("tsl exit tid update: %d \n", ReadyQueue->head->tcb->tid);
        ReadyQueue->head->tcb->state = ENDED;
        printf("tsl exit state update: %d \n", ReadyQueue->head->tcb->state);
        ReadyQueue->numOfThreads = ReadyQueue->numOfThreads - 1;
        tsl_yield(0);
    } else {
        printf("There is no running thread! \n");
    }
    return (0);
}

int tsl_join(int tid) {
    if (!library_state || !ReadyQueue) {
        printf("library is not initialized.\n");
        return -1; 
    }

    TCBNode *current_node = ReadyQueue->head;
    TCBNode *prev_node = NULL;

    // search for the target thread in the ready queue
    while (current_node != NULL) {
        if (current_node->tcb->tid == tid) {
            // wait until the target thread terminates
            if (current_node->tcb->state != ENDED) {
                tsl_yield(tid);
            } else {
                // target thread terminated, deallocating its resources
                if (prev_node == NULL) {
                    ReadyQueue->head = current_node->next; // remove from the head of the queue
                } else {
                    prev_node->next = current_node->next;
                }
                free(current_node->tcb->stack);
                free(current_node->tcb);
                free(current_node);
                printf("thread tid: %d joined \n", tid);
                return tid; // rreturn the tid of the joined (target) thread
            }
        }
        prev_node = current_node;
        current_node = current_node->next;
    }
    return -1;
}

int tsl_cancel(int tid) {
    if (!library_state || !ReadyQueue) {
        printf( "library is not initialized.\n");
        return -1; 
    }

    TCBNode *current_node = ReadyQueue->head;
    TCBNode *prev_node = NULL;

    // Search for the target thread in the ready queue
    while (current_node != NULL) {
        if (current_node->tcb->tid == tid) {
            // Mark the target thread as terminated
            current_node->tcb->state = ENDED;

            // Remove the target thread from the ready queue
            if (prev_node == NULL) {
                ReadyQueue->head = current_node->next; // Remove from the head of the queue
            } else {
                prev_node->next = current_node->next;
            }

            // Deallocate resources associated with the terminated thread
            free(current_node->tcb->stack);
            free(current_node->tcb);
            free(current_node);

            printf("Thread tid: %d has been cancelled.\n", tid);
            return 0; // Success
        }
        prev_node = current_node;
        current_node = current_node->next;
    }
    //  no thread with the indicated tid is found
    printf( "Thread with tid %d not found.\n", tid);
    return -1; // thread not found
}


int tsl_gettid() {
    if (ReadyQueue->head != NULL) {
        return ReadyQueue->head->tcb->tid;
    } else {
        return TSL_ERROR;
    }
}
