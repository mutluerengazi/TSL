#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE 1024*64

ucontext_t main_context, thread_context;

// Thread function
void thread_function() {
    printf("Hello from the thread function!\n");
    // Switch back to main context
    swapcontext(&thread_context, &main_context);
}

int main() {
    // Get the current context as the main context
    getcontext(&main_context);

    // Prepare the thread context
    getcontext(&thread_context);
    thread_context.uc_link = &main_context; // Set where to go after the thread_function finishes
    thread_context.uc_stack.ss_sp = malloc(STACK_SIZE); // Allocate stack for the new context
    thread_context.uc_stack.ss_size = STACK_SIZE;
    thread_context.uc_stack.ss_flags = 0;
    if (thread_context.uc_stack.ss_sp == 0) {
        perror("Failed to allocate stack");
        exit(1);
    }

    // Create the new context for thread_function, it will execute after swapcontext
    makecontext(&thread_context, thread_function, 0);

    // Switch to the thread context, which starts thread_function
    printf("Switching to the thread function...\n");
    swapcontext(&main_context, &thread_context);

    // Cleanup
    free(thread_context.uc_stack.ss_sp);

    printf("Back to the main function\n");

    return 0;
}
