#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdbool.h>

void must_init(bool test, const char *description);
void safe_exit(int code);

#endif