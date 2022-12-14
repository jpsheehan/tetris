#include <allegro5/allegro_primitives.h>

#include "field.h"
#include "score.h"
#include "audio.h"

static CELL field[FIELD_H][FIELD_W];

void field_draw_cell(int x, int y, ALLEGRO_COLOR c)
{
    if (x < 0 || x >= FIELD_W || y < 0 || y >= FIELD_H)
        return;

    int buffer_x = FIELD_MARGIN_X + x * MINO_W;
    int buffer_y = FIELD_MARGIN_Y + y * MINO_H;

    mino_draw_cell(buffer_x, buffer_y, c, 1);
}

void field_draw_mino(PIECE piece, int rotation, int x, int y, ALLEGRO_COLOR c)
{
    for (int i = 0; i < 4; i++)
    {
        int cell_x, cell_y;
        mino_unmap_xy_offsets(piece, rotation, i, &cell_x, &cell_y);

        field_draw_cell(x + cell_x, y + cell_y, c);
    }
}

void field_init(void)
{
    for (int y = 0; y < FIELD_H; y++)
        for (int x = 0; x < FIELD_W; x++)
        {
            field[y][x].used = false;
            field[y][x].c = al_map_rgb_f((float)x / (float)FIELD_H, (float)y / (float)FIELD_W, (float)(rand()) / (float)RAND_MAX);
        }
}

void shift_upper_rows_down(int j)
{
    for (int y = j - 1; y >= 0; y--)
    {
        for (int x = 0; x < FIELD_W; x++)
        {
            field[y + 1][x].used = field[y][x].used;
            field[y + 1][x].c = field[y][x].c;
        }
    }
}

void clear_row(int j)
{
    for (int i = 0; i < FIELD_W; i++)
        field[j][i].used = false;
}

void field_update(void)
{
    int num_rows_cleared = 0;

    for (int j = 0; j < FIELD_H; j++)
    {
        bool row_is_cleared = true;
        for (int i = 0; i < FIELD_W; i++)
        {
            if (!field[j][i].used)
            {
                row_is_cleared = false;
                break;
            }
        }

        if (row_is_cleared)
        {
            num_rows_cleared++;
            clear_row(j);
            shift_upper_rows_down(j);
        }
    }

    if (num_rows_cleared > 0)
    {
        score_add_cleared_lines(num_rows_cleared);
        audio_play_sfx(SFX_LINE_CLEAR);
    }
}

void field_draw(bool show_stack)
{
    int border_w = 1;
    ALLEGRO_COLOR border_c = al_map_rgb(0x98, 0x00, 0xff);

    al_draw_rectangle(FIELD_MARGIN_X, FIELD_MARGIN_Y - border_w, FIELD_MARGIN_X + FIELD_W * MINO_W + border_w, FIELD_MARGIN_Y + FIELD_H * MINO_H, border_c, border_w);

    if (!show_stack)
        return;
        
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