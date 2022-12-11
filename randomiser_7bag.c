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

// void randomiser_draw(void)
// {
//     int offset_x = BUFFER_W - 30;
//     int offset_y = 10;

//     for (int j = 0; j < 6; j++)
//     {
//         PIECE piece = bag_pieces[(j + bag_idx) % RANDOMISER_BUFFER_MAX];
//         for (int i = 0; i < 4; i++)
//         {
//             int x = offset_x + rotations[piece][0][i].x * CELL_W;
//             int y = offset_y + rotations[piece][0][i].y * CELL_H;
//             field_draw_cell_raw(x, y, player_get_default_colour(piece));
//         }
//     }
// }