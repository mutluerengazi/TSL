#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

static int context_set = 0; // Add a global variable to track if the context has been set
int globalvar = 21;

int main() {
    int b = 10;
    ucontext_t context;
    ucontext_t contextSecond;


    // Save the current context
    getcontext(&context);
    printf("this is first.\n");

    getcontext(&contextSecond);
    
    printf("this is second.\n");
    printf("Screenshot globalvar = %d\n", globalvar);
    printf("Screenshot b = %d\n", b);

    if (context_set == 0) {
        // This block will run both initially and after the context is set below
        printf("Before setting the saved context.\nThis will only be executed once.\n");

        context_set = 1; // Mark the context as set

        // Do some operations
        int a = 5;
        printf("Performing some operations... a = %d\n", a);

        globalvar = globalvar + globalvar;
        b++;
        printf("loop globalvar = %d\n", globalvar);

        // Restore the saved context
        setcontext(&contextSecond);

        // The execution will jump back to getcontext() after this line
    } else {
        // This block runs after the context has been restored for the second time
        printf("After setting the context.\nThis is after restoring the context.\n");
    }

    return 0;
}
