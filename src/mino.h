#ifndef MINO_H_
#define MINO_H_

#include <allegro5/allegro.h>

#define MINO_W 20
#define MINO_H 20

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

typedef struct POINT
{
    int x, y;
} POINT;

ALLEGRO_COLOR mino_get_default_colour(PIECE piece);
void mino_unmap_xy_offsets(PIECE piece, int rotation, int idx, int* x, int* y);
void mino_draw(PIECE piece, int rotation, int offset_x, int offset_y, ALLEGRO_COLOR c, float scale);
void mino_draw_cell(int x, int y, ALLEGRO_COLOR c, float scale);
POINT* mino_get_kick_data(PIECE piece, int start_rotation, bool ccw);

#endif