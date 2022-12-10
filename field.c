#include <allegro5/allegro_primitives.h>

#include "field.h"

static CELL field[FIELD_H][FIELD_W];

void field_draw_cell(int x, int y, ALLEGRO_COLOR c)
{
    if (x < 0 || x >= FIELD_W || y < 0 || y >= FIELD_H)
        return;

    int buffer_x = FIELD_MARGIN_X + x * CELL_W;
    int buffer_y = FIELD_MARGIN_Y + y * CELL_H;
    al_draw_filled_rectangle(buffer_x, buffer_y, buffer_x + CELL_W, buffer_y + CELL_H, c);
}

void field_init()
{
    for (int y = 0; y < FIELD_H; y++)
        for (int x = 0; x < FIELD_W; x++)
        {
            field[y][x].used = false;
            field[y][x].c = al_map_rgb_f((float)x / (float)FIELD_H, (float)y / (float)FIELD_W, (float)(rand()) / (float)RAND_MAX);
        }
}

void field_draw()
{
    int border_w = 1;
    ALLEGRO_COLOR border_c = al_map_rgb(0x98, 0x00, 0xff);

    al_draw_rectangle(FIELD_MARGIN_X, FIELD_MARGIN_Y - border_w, FIELD_MARGIN_X + FIELD_W * CELL_W + border_w, FIELD_MARGIN_Y + FIELD_H * CELL_H, border_c, border_w);

    for (int j = 0; j < FIELD_H; j++)
        for (int i = 0; i < FIELD_W; i++)
        {
            CELL *cell = &field[j][i];

            if (cell->used)
            {
                field_draw_cell(i, j, cell->c);
            }
        }
}

bool field_get_used_or_default(int x, int y, bool default_)
{
    if (x < 0 || x >= FIELD_W || y < 0 || y >= FIELD_H)
        return default_;
    return field[y][x].used;
}

void field_set_used_safely(int x, int y, ALLEGRO_COLOR c)
{
    if (x < 0 || x >= FIELD_W ||
        y < 0 || y >= FIELD_H)
    {
        return;
    }

    field[y][x].used = true;
    field[y][x].c = c;
}