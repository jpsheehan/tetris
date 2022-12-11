#ifndef MINO_H_
#define MINO_H_

#include <allegro5/allegro.h>

typedef enum PIECE
{
    O,
    T,
    I,
    J,
    L,
    S,
    Z,
    PIECE_MAX
} PIECE;

ALLEGRO_COLOR mino_get_default_colour(PIECE piece);

void mino_unmap_xy_offsets(PIECE piece, int rotation, int idx, int* x, int* y);

#endif