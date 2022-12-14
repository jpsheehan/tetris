#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "mino.h"

#define FPS 60

#define ASSERT_RANGE(x, min, max, name)                                         \
    do                                                                          \
    {                                                                           \
        if ((x) < (min) || (x) >= (max))                                        \
        {                                                                       \
            char __message[100] = {0};                                          \
            snprintf(__message, 100, "invalid %s in %s", (name), __FUNCTION__); \
            safe_exit(__message, 1);                                            \
        }                                                                       \
    } while (0)

#define ASSERT_PIECE(piece) ASSERT_RANGE(piece, 0, PIECE_MAX, "piece")
#define ASSERT_ROTATION(rot) ASSERT_RANGE(rot, 0, 4, "rotation")

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

void must_init(bool test, const char *description);
void safe_exit(const char* description, int code);

#endif