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

TSL_Library_State *library_state = NULL;


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

    // Set the scheduling algorithm
    library_state->scheduling_algorithm = salg;

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
    library_state->main_thread_tcb->tid = 1; // Assigning a unique identifier to the main thread
    library_state->main_thread_tcb->state = TSL_RUNNING; // Assuming RUNNING is a defined state

    // If necessary, initialize your ready queue and other data structures here

    return 0; // Success
}



int tsl_create_thread(void (*tsf)(void *), void *targ)
{
    /*
        The new thread will start its execution at
        the specified function tsf, i.e., the thread start function (or root function).
        The application will define this thread start function, and its address will be
        passed to tsl create thread() as the first argument. The start function
        can take one parameter of type void *. Therefore, the second argument to
        the tsl create thread() function, i.e., targ, is a pointer that can point
        to a value or structure that will be passed to the thread start function. If
        nothing is to be passed, NULL can be specified as the second argument to
        tsl create thread().
        As the return value, the integer identifier (tid) of the created thread will
        be returned. Your library will assign a unique positive integer identifier to
        every thread, including the main thread.
        If the new thread could not be created due to some reason, then -1
        (TSL ERROR) will be returned as the return value.
        After creating the new thread, the main thread can continue running
        until it calls the tsl yield() function.

        Not that you will mainly call getcontext() from inside tsl yield(),
        which is doing a context switch to another thread. You will also call it in tsl -
        init() and tsl create thread() where you will allocate and initialize a TCB
        for the main thread and a new thread
    */
    return (0);
}



int tsl_yield(int tid)
{
    /*
        With this function, a running caller (calling)
        thread will yield (give) the cpu to some other thread. A context-switch from
        the caller thread to another thread should happen.
        If the tid parameter is equal to the tid of an existing thread, then that
        thread should be selected as the thread to run next. If tid parameter is equal
        to the special value 0 (TSL ANY) the next thread to run will be selected from
        the ready queue according to a scheduling algorithm.
        After selecting the next thread to run, the selected thread’s saved con-
        text will be reloaded to the CPU and the selected thread will start execution.
        If there is no other thread in the ready state other than the calling
        thread, the scheduler needs to select the calling thread to run next. That
        means, in this case, your library will save the calling thread context first and
        then restore it.
        If tid parameter is a positive integer but there is no ready thread
        with that tid, the function will return immediately without yielding to any
        thread. In this case it will return -1 as the return value. Otherwise, the
        return value should be the tid of the thread to whom the cpu is yielded.
        Note that this function will not return immediately to the caller, but
        will return later when the calling thread (caller) is re-scheduled again.

        Not that you will mainly call getcontext() from inside tsl yield(),
        which is doing a context switch to another thread. You will also call it in tsl -
        init() and tsl create thread() where you will allocate and initialize a TCB
        for the main thread and a new thread
     */
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


int tsl_gettid()
{
    return (0);
}