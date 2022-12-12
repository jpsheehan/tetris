#include <time.h>

#include "randomiser.h"
#include "display.h"
#include "mino.h"

#define RANDOMISER_BUFFER_MAX 14

static PIECE bag_pieces[RANDOMISER_BUFFER_MAX];
static int bag_idx;

void fill_bag(int offset)
{
    for (int i = 0; i < 7; i++)
    {
        while (1) {
            PIECE piece = (PIECE)(rand() % PIECE_MAX);
            bool not_found = true;
            for (int j = offset; j < i + offset; j++) {
                not_found &= bag_pieces[j] != piece;
            }
            if (not_found) {
                bag_pieces[i + offset] = piece;
                break;
            }
        }
    }
}

void randomiser_init(void)
{
    srand(time(NULL));
    bag_idx = 0;
    fill_bag(0);
    fill_bag(7);
}

void randomiser_deinit(void)
{
}

PIECE randomiser_next(void)
{
    if (bag_idx == 7) {
        fill_bag(0);
    }
    else if (bag_idx == 14) {
        bag_idx = 0;
        fill_bag(7);
    }
    
    return bag_pieces[bag_idx++];
}

void randomiser_peek(int* n, PIECE* buffer)
{
    for (int i = 0; i < 6; i++) {
        buffer[i] = bag_pieces[(i + bag_idx) % RANDOMISER_BUFFER_MAX];
    }
    *n = 6;
}
