#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

static int context_set = 0; // Add a global variable to track if the context has been set

int main() {
    ucontext_t context;

    // Save the current context
    getcontext(&context);

    if (context_set == 0) {
        // This block will run both initially and after the context is set below
        printf("Before setting the saved context.\nThis will only be executed once.\n");

        context_set = 1; // Mark the context as set

        // Do some operations
        int a = 5;
        printf("Performing some operations... a = %d\n", a);

        // Restore the saved context
        setcontext(&context);

        // The execution will jump back to getcontext() after this line
    } else {
        // This block runs after the context has been restored for the second time
        printf("After setting the context.\nThis is after restoring the context.\n");
    }

    return 0;
}
