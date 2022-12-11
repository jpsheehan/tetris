#include <allegro5/allegro_primitives.h>

#include "mino.h"
#include "utils.h"
#include "field.h"

typedef struct POINT
{
    int x, y;
} POINT;

static unsigned char colours[PIECE_MAX][3] = {
    {0xEF, 0xE7, 0x10}, // O
    {0xc2, 0x2d, 0xd2}, // T
    {0x1e, 0xcb, 0xe1}, // I
    {0x16, 0x0b, 0xf4}, // J
    {0xff, 0xc2, 0x00}, // L
    {0x41, 0xff, 0x00}, // S
    {0xe7, 0x18, 0x23}, // Z
};

static POINT rotations[PIECE_MAX][4][4] = {
    {
        // O
        {
            //  ##
            //  ##
            //
            {.x = 1, .y = 0},
            {.x = 2, .y = 0},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
        },
        {
            //  ##
            //  ##
            //
            {.x = 1, .y = 0},
            {.x = 2, .y = 0},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
        },
        {
            //  ##
            //  ##
            //
            {.x = 1, .y = 0},
            {.x = 2, .y = 0},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
        },
        {
            //  ##
            //  ##
            //
            {.x = 1, .y = 0},
            {.x = 2, .y = 0},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
        },
    },
    {
        // T
        {
            //  #
            // ###
            {.x = 1, .y = 0},
            {.x = 0, .y = 1},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
        },
        {
            //  #
            //  ##
            //  #
            {.x = 1, .y = 0},
            {.x = 1, .y = 1},
            {.x = 1, .y = 2},
            {.x = 2, .y = 1},
        },
        {
            //
            // ###
            //  #
            {.x = 1, .y = 2},
            {.x = 0, .y = 1},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
        },
        {
            //  #
            // ##
            //  #
            {.x = 1, .y = 0},
            {.x = 1, .y = 1},
            {.x = 1, .y = 2},
            {.x = 0, .y = 1},
        },
    },
    {
        // I
        {
            //
            // ####
            //
            //
            {.x = 0, .y = 1},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
            {.x = 3, .y = 1},
        },
        {
            //   #
            //   #
            //   #
            //   #
            {.x = 2, .y = 0},
            {.x = 2, .y = 1},
            {.x = 2, .y = 2},
            {.x = 2, .y = 3},
        },
        {
            //
            //
            // ####
            //
            {.x = 0, .y = 2},
            {.x = 1, .y = 2},
            {.x = 2, .y = 2},
            {.x = 3, .y = 2},
        },
        {
            //  #
            //  #
            //  #
            //  #
            {.x = 1, .y = 0},
            {.x = 1, .y = 1},
            {.x = 1, .y = 2},
            {.x = 1, .y = 3},
        },
    },
    {
        // J
        {
            // #
            // ###
            //
            {.x = 0, .y = 0},
            {.x = 0, .y = 1},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
        },
        {
            //  ##
            //  #
            //  #
            {.x = 1, .y = 0},
            {.x = 1, .y = 1},
            {.x = 1, .y = 2},
            {.x = 2, .y = 0},
        },
        {
            //
            // ###
            //   #
            {.x = 2, .y = 2},
            {.x = 0, .y = 1},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
        },
        {
            //  #
            //  #
            // ##
            {.x = 1, .y = 0},
            {.x = 1, .y = 1},
            {.x = 1, .y = 2},
            {.x = 0, .y = 2},
        },
    },
    {
        // L
        {
            //   #
            // ###
            //
            {.x = 2, .y = 0},
            {.x = 0, .y = 1},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
        },
        {
            //  #
            //  #
            //  ##
            {.x = 1, .y = 0},
            {.x = 1, .y = 1},
            {.x = 1, .y = 2},
            {.x = 2, .y = 2},
        },
        {
            //
            // ###
            // #
            {.x = 0, .y = 2},
            {.x = 0, .y = 1},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
        },
        {
            // ##
            //  #
            //  #
            {.x = 1, .y = 0},
            {.x = 1, .y = 1},
            {.x = 1, .y = 2},
            {.x = 0, .y = 0},
        },
    },
    {
        // S
        {
            //  ##
            // ##
            //
            {.x = 0, .y = 1},
            {.x = 1, .y = 1},
            {.x = 1, .y = 0},
            {.x = 2, .y = 0},
        },
        {
            //  #
            //  ##
            //   #
            {.x = 1, .y = 0},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
            {.x = 2, .y = 2},
        },
        {
            //
            //  ##
            // ##
            {.x = 0, .y = 2},
            {.x = 1, .y = 2},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
        },
        {
            // #
            // ##
            //  #
            {.x = 0, .y = 0},
            {.x = 0, .y = 1},
            {.x = 1, .y = 1},
            {.x = 1, .y = 2},
        },
    },
    {
        // Z
        {
            // ##
            //  ##
            //
            {.x = 0, .y = 0},
            {.x = 1, .y = 0},
            {.x = 1, .y = 1},
            {.x = 2, .y = 1},
        },
        {
            //   #
            //  ##
            //  #
            {.x = 2, .y = 0},
            {.x = 2, .y = 1},
            {.x = 1, .y = 1},
            {.x = 1, .y = 2},
        },
        {
            //
            // ##
            //  ##
            {.x = 0, .y = 1},
            {.x = 1, .y = 1},
            {.x = 1, .y = 2},
            {.x = 2, .y = 2},
        },
        {
            //  #
            // ##
            // #
            {.x = 1, .y = 0},
            {.x = 1, .y = 1},
            {.x = 0, .y = 1},
            {.x = 0, .y = 2},
        },
    },
};

ALLEGRO_COLOR mino_get_default_colour(PIECE piece)
{
    ASSERT_PIECE(piece);

    unsigned char r = colours[piece][0];
    unsigned char g = colours[piece][1];
    unsigned char b = colours[piece][2];

    return al_map_rgb(r, g, b);
}

void mino_unmap_xy_offsets(PIECE piece, int rotation, int idx, int *x, int *y)
{
    ASSERT_PIECE(piece);
    ASSERT_ROTATION(rotation);
    ASSERT_RANGE(idx, 0, 4, "mino offset");

    *x = rotations[piece][rotation][idx].x;
    *y = rotations[piece][rotation][idx].y;
}

void mino_draw(PIECE piece, int rotation, int offset_x, int offset_y, ALLEGRO_COLOR c, float scale)
{
    for (int i = 0; i < 4; i++)
    {
        int x = offset_x + rotations[piece][rotation][i].x * MINO_W * scale;
        int y = offset_y + rotations[piece][rotation][i].y * MINO_H * scale;
        mino_draw_cell(x, y, c, scale);
    }
}

void mino_draw_cell(int x, int y, ALLEGRO_COLOR c, float scale)
{
    al_draw_filled_rectangle(x, y, x + MINO_W * scale, y + MINO_H * scale, c);
    float r, g, b;
    al_unmap_rgb_f(c, &r, &g, &b);
    r *= 0.7;
    g *= 0.7;
    b *= 0.7;
    al_draw_rectangle(x, y, x + MINO_W * scale, y + MINO_H * scale, al_map_rgb_f(r, g, b), 1);
}