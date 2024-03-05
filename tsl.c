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

int tsl_init(int salg);
int tsl_create_thread (void (*tsf)(void *), void *targ);
int tsl_yield (int tid);
int tsl_exit();
int tsl_join(int tid);
int tsl_cancel(int tid);
int tsl_gettid();


int tsl_init (int salg)
{
    /*
        An application will call this function exactly once before creating any threads.
        The parameter salg is used to indicate the scheduling algorithm the library
        will use. On success, 0 will be return. On failure, -1 (TSL ERROR) will be
        returned

        As part of this initialization,
        a ready queue structure (runqueue structure) should be created and initialized
        as well. It will keep a list of TCBs corresponding to the threads that are in ready
        state. If you wish you can create and initialize other queues. It is up to you how to
        manage the set of TCBs of the threads that can be in various states. You can keep
        all TCBs in a single data structure or in multiple data structures.
        You will also need to allocate a TCB for the main thread of the application
        (process). Its state will be RUNNING initially. You will also assign a unique thread
        identifier to the main thread. It can be 1)

        Not that you will mainly call getcontext() from inside tsl yield(),
        which is doing a context switch to another thread. You will also call it in tsl -
        init() and tsl create thread() where you will allocate and initialize a TCB
        for the main thread and a new thread
     */

    return (0);
    // we put return(0) as a placeholder.
    // read project about what to return.
    //and change return() accordingly. 
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