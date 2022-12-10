#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdbool.h>

#define FPS 60

void must_init(bool test, const char *description);
void safe_exit(const char* description, int code);

#endif