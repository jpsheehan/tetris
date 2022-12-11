#include <stdio.h>

#include "utils.h"
#include "display.h"

void must_init(bool test, const char *description)
{
    if (test)
        return;
    
    printf("couldn't initialise %s\n", description);
    safe_exit(NULL, 1);
}

void safe_exit(const char *message, int code)
{
    if (message != NULL) {
        printf("%s\n", message);
    }

    disp_deinit();

    exit(code);
}