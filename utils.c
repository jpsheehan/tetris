#include <stdio.h>

#include "utils.h"
#include "display.h"

void must_init(bool test, const char *description)
{
    if (test)
        return;
    
    printf("couldn't initialise %s\n", description);
    safe_exit(1);
}

void safe_exit(int code)
{
    disp_deinit();

    exit(code);
}