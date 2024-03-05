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


int 
tsl_init ( int salg)
{
    return (0);
    // we put return(0) as a placeholder.
    // read project about what to return.
    //and change return() accordingly. 
}



int
tsl_create_thread(void (*tsf)(void *), void *targ)
{
    return (0);
}



int
tsl_yield(int tid)
{
    return (0);
}


int 
tsl_exit()
{
    return (0);
}

int
tsl_join(int tid)
{
    return (0);
}


int
tsl_cancel(int tid)
{
    return (0);
}


int
tsl_gettid()
{
    return (0);
}