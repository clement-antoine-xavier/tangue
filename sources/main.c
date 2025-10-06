#include <stdio.h>
#include <stdlib.h>
#include "../headers/application.h"

int main(int argc, char *argv[]) {
    // Initialize application context
    application_context_t app;
    
    if (application_init(&app) != 0) {
        fprintf(stderr, "Failed to initialize application\n");
        return EXIT_FAILURE;
    }
    
    // Run the main application loop
    application_run(&app);
    
    // Cleanup
    application_cleanup(&app);
    
    return EXIT_SUCCESS;
}