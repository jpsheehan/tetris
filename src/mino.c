#include <allegro5/allegro_primitives.h>

#include "mino.h"
#include "utils.h"
#include "field.h"

static unsigned char colours[PIECE_MAX][3] = {
    {0xEF, 0xE7, 0x10}, // O
    {0xc2, 0x2d, 0xd2}, // T
    {0x1e, 0xcb, 0xe1}, // I
    {0x66, 0x44, 0xff}, // J
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
            //
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

// from https://tetris.fandom.com/wiki/SRS#Wall_Kicks
// note that the y-values have been inverted due to the coordinate system I'm using

#define CW 0
#define CCW 1

static POINT jltsz_kicks[4][2][4] = {
    {
        // Rotation 0
        {
            // CW (to Rotation 1)
            {.x = -1, .y = 0},
            {.x = -1, .y = -1},
            {.x = 0, .y = 2},
            {.x = -1, .y = 2},
        },
        {
            // CCW (to Rotation 3)
            {.x = 1, .y = 0},
            {.x = 1, .y = -1},
            {.x = 0, .y = 2},
            {.x = 1, .y = 2},
        },
    },
    {
        // Rotation 1
        {
            // CW (to Rotation 2)
            {.x = 1, .y = 0},
            {.x = 1, .y = 1},
            {.x = 0, .y = -2},
            {.x = 1, .y = -2},
        },
        {
            // CCW (to rotation 0)
            {.x = 1, .y = 0},
            {.x = 1, .y = 1},
            {.x = 0, .y = -2},
            {.x = 1, .y = -2},
        },
    },
    {
        // Rotation 2
        {
            // CW (to Rotation 3)
            {.x = 1, .y = 0},
            {.x = 1, .y = -1},
            {.x = 0, .y = 2},
            {.x = 1, .y = 2},
        },
        {
            // CCW (to Rotation 1)
            {.x = -1, .y = 0},
            {.x = -1, .y = -1},
            {.x = 0, .y = 2},
            {.x = -1, .y = 2},
        },
    },
    {
        // Rotation 3
        {
            // CW (to Rotation 0)
            {.x = -1, .y = 0},
            {.x = -1, .y = 1},
            {.x = 0, .y = -2},
            {.x = -1, .y = -2},
        },
        {
            // CCW (to Rotation 2)
            {.x = -1, .y = 0},
            {.x = -1, .y = 1},
            {.x = 0, .y = -2},
            {.x = -1, .y = -2},
        },
    },
};

static POINT i_kicks[4][2][4] = {
    {
        // Rotation 0
        {
            // CW (to Rotation 1)
            {.x = -2, .y = 0},
            {.x = 1, .y = 0},
            {.x = -2, .y = 1},
            {.x = 1, .y = -2},
        },
        {
            // CCW (to Rotation 3)
            {.x = -1, .y = 0},
            {.x = 2, .y = 0},
            {.x = -1, .y = -2},
            {.x = 2, .y = 1},
        },
    },
    {
        // Rotation 1
        {
            // CW (to Rotation 2)
            {.x = -1, .y = 0},
            {.x = 2, .y = 0},
            {.x = -1, .y = -2},
            {.x = 2, .y = 1},
        },
        {
            // CCW (to rotation 0)
            {.x = 2, .y = 0},
            {.x = -1, .y = 0},
            {.x = 2, .y = -1},
            {.x = -1, .y = 2},
        },
    },
    {
        // Rotation 2
        {
            // CW (to Rotation 3)
            {.x = 2, .y = 0},
            {.x = -1, .y = 0},
            {.x = 2, .y = -1},
            {.x = -1, .y = 2},
        },
        {
            // CCW (to Rotation 1)
            {.x = 1, .y = 0},
            {.x = -2, .y = 0},
            {.x = 1, .y = 2},
            {.x = -2, .y = -1},
        },
    },
    {
        // Rotation 3
        {
            // CW (to Rotation 0)
            {.x = 1, .y = 0},
            {.x = -2, .y = 0},
            {.x = 1, .y = 2},
            {.x = -2, .y = -1},
        },
        {
            // CCW (to Rotation 2)
            {.x = -2, .y = 0},
            {.x = 1, .y = 0},
            {.x = -2, .y = 1},
            {.x = 1, .y = -2},
        },
    },
};

POINT* mino_get_kick_data(PIECE piece, int start_rotation, bool ccw)
{
    ASSERT_PIECE(piece);
    ASSERT_ROTATION(start_rotation);

    switch (piece)
    {
        case J:
        case L:
        case T:
        case S:
        case Z:
            return jltsz_kicks[start_rotation][ccw ? 1 : 0];
        case I:
            return i_kicks[start_rotation][ccw ? 1 : 0];
        default:
            return NULL;
    }
}

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
    float r, g, b, a;

    al_unmap_rgba_f(c, &r, &g, &b, &a);
    r *= 0.7;
    g *= 0.7;
    b *= 0.7;
    ALLEGRO_COLOR lighter = al_map_rgba_f(r, g, b, a);

    ALLEGRO_VERTEX v[] = {
        { .x = x, .y = y, .z = 0, .color = c },
        { .x = x + MINO_W * scale, .y = y, .z = 0, .color = lighter },
        { .x = x, .y = y + MINO_W * scale, .z = 0, .color = lighter },
        { .x = x + MINO_W * scale, .y = y + MINO_W * scale, .z = 0, .color = c },
    };
    al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);

    al_unmap_rgba_f(c, &r, &g, &b, &a);
    r *= 0.7;
    g *= 0.7;
    b *= 0.7;
    ALLEGRO_COLOR darker = al_map_rgba_f(r, g, b, a);
    
    al_draw_rectangle(x, y, x + MINO_W * scale, y + MINO_H * scale, darker, 1);
}