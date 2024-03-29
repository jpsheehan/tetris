#include <stdbool.h>
#include <allegro5/allegro_primitives.h>

#include "field.h"
#include "score.h"
#include "audio.h"
#include "utils.h"
#include "player.h"

static CELL field[FIELD_H][FIELD_W] = {0};

static bool requires_update = false;
static bool is_perfect_clear(void);
static int cleared_rows[4] = {0};
static int num_cleared_rows = 0;

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
    num_cleared_rows = 0;
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

void field_clear_rows(void)
{
    for (int i = 0; i < num_cleared_rows; i++)
    {
        int row = cleared_rows[i];
        clear_row(row);
        shift_upper_rows_down(row);
    }
    num_cleared_rows = 0;
}

void set_requires_update(void)
{
    requires_update = true;
}

void field_update(void)
{
    if (!requires_update)
        return;

    num_cleared_rows = 0;

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
            cleared_rows[num_cleared_rows++] = j;
        }
    }

    if (num_cleared_rows > 0)
    {
        switch (player_get_tspin_state())
        {
        case TS_NONE:
            switch (num_cleared_rows)
            {
            case 1:
                game_show_bonus(SINGLE);
                break;
            case 2:
                game_show_bonus(DOUBLE);
                break;
            case 3:
                game_show_bonus(TRIPLE);
                break;
            case 4:
                game_show_bonus(TETRIS);
                break;
            default:
                safe_exit("Invalid number of lines cleared", 1);
                break;
            }
            break;
        case TS_PROPER:
            switch (num_cleared_rows)
            {
            case 1:
                game_show_bonus(TSPIN_SINGLE);
                break;
            case 2:
                game_show_bonus(TSPIN_DOUBLE);
                break;
            case 3:
                game_show_bonus(TSPIN_TRIPLE);
                break;
            default:
                safe_exit("Invalid number of lines cleared for tspin proper", 1);
                break;
            }
            break;
        case TS_MINI:
            switch (num_cleared_rows)
            {
            case 1:
                game_show_bonus(TSPIN_MINI_SINGLE);
                break;
            default:
                // for some reason we can hit this :(
                // safe_exit("Invalid number of lines cleared for tspin mini", 1);
                break;
            }
            break;
        }

        // perfect clear points are bonus points and always awarded
        if (is_perfect_clear())
        {
            switch (num_cleared_rows)
            {
            case 1:
                game_show_bonus(PERFECT_CLEAR_SINGLE);
                break;
            case 2:
                game_show_bonus(PERFECT_CLEAR_DOUBLE);
                break;
            case 3:
                game_show_bonus(PERFECT_CLEAR_TRIPLE);
                break;
            case 4:
                game_show_bonus(PERFECT_CLEAR_TETRIS);
                break;
            default:
                safe_exit("Invalid number of lines cleared", 1);
                break;
            }
        }
    }
    else
    {
        switch (player_get_tspin_state())
        {
        case TS_PROPER:
            game_show_bonus(TSPIN_SINGLE_NO_LINES);
            break;
        case TS_MINI:
            game_show_bonus(TSPIN_MINI_SINGLE_NO_LINES);
            break;
        case TS_NONE:
            break;
        }
    }

    requires_update = false;
}

void field_draw(bool show_stack)
{
    int border_w = 2;
    ALLEGRO_COLOR border_c = al_map_rgb(0x98, 0x00, 0xff); // Kairo's purple colour

    // draw a transparent background rectangle
    al_draw_filled_rectangle(FIELD_MARGIN_X - 1, FIELD_MARGIN_Y - border_w, FIELD_MARGIN_X + FIELD_W * MINO_W + border_w, FIELD_MARGIN_Y + FIELD_H * MINO_H + 1, al_map_rgba_f(0, 0, 0, 0.6));

    // draw a border
    al_draw_rectangle(FIELD_MARGIN_X - 1, FIELD_MARGIN_Y - border_w, FIELD_MARGIN_X + FIELD_W * MINO_W + border_w, FIELD_MARGIN_Y + FIELD_H * MINO_H + 1, border_c, border_w);

    if (!show_stack)
        return;

    for (int j = FIELD_H - 1; j >= 0; j--)
    {
        // check if the row has been marked as cleared
        bool is_row_cleared = false;
        for (int i = 0; i < num_cleared_rows; i++) {
            if (cleared_rows[i] == j) {
                is_row_cleared = true;
                break;
            }
        }

        // determine an appropriate alpha value for the row
        float row_a = 1.0;
        if (is_row_cleared) {
            row_a = 1.0 - CLAMP((float)player_get_clear_timer_count() / CLEAR_TIMER_RESOLUTION, 0.0, 1.0);
        }

        for (int i = 0; i < FIELD_W; i++)
        {
            CELL *cell = &field[j][i];

            if (cell->used)
            {
                float r, g, b, a;
                al_unmap_rgba_f(cell->c, &r, &g, &b, &a);
                field_draw_cell(i, j, al_premul_rgba_f(r, g, b, a == 1.0 ? row_a : a));
            }
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

static bool is_perfect_clear(void)
{
    for (int j = 0; j < FIELD_H; j++)
        for (int i = 0; i < FIELD_W; i++)
        {
            CELL *cell = &field[j][i];

            if (cell->used)
            {
                return false;
            }
        }
    return true;
}