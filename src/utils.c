#include <stdio.h>

#include "utils.h"
#include "display.h"
#include "asset_loader.h"

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

    asset_loader_unload_all();
    asset_loader_deinit_allegro();

    exit(code);
}

void unsafe_exit(const char *message, int code)
{
    if (message != NULL) {
        printf("%s\n", message);
    }

    exit(code);
}