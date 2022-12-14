#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "hud.h"
#include "score.h"
#include "display.h"
#include "mino.h"
#include "randomiser.h"
#include "player.h"

#define FIRST_X 10
#define FIRST_Y 10

#define SECOND_X (BUFFER_W - 70)
#define SECOND_Y 40

#define THIRD_Y (BUFFER_H - 70)

static ALLEGRO_FONT *font = NULL;
static long score_display;

void draw_score(void);
void draw_randomiser(void);
void draw_held_piece(void);
void draw_level(void);
void draw_remaining_lines(void);
void draw_lines_cleared(void);

void hud_init(void)
{
    font = al_create_builtin_font();
    score_display = 0;
}

void hud_deinit(void)
{
    if (font != NULL)
    {
        al_destroy_font(font);
        font = NULL;
    }
}

void hud_update(void)
{
    if (score_display < score_get())
    {
        score_display += 25;
        if (score_display > score_get())
            score_display = score_get();
    }
}

void hud_draw(bool show_minos)
{
    draw_score();
    if (show_minos) {
        draw_randomiser();
        draw_held_piece();
    }
    draw_level();
    draw_remaining_lines();
    draw_lines_cleared();
}

void draw_score(void)
{
    al_draw_textf(font, al_map_rgb_f(1, 1, 1), 10, FIRST_Y, 0, "%06ld", score_display);
}

void draw_randomiser(void)
{
    al_draw_text(font, al_map_rgb_f(1, 1, 1), SECOND_X, SECOND_Y, 0, "Next");

    int gap_y = 20;
    PIECE bag[6];
    int num_read = 0;

    randomiser_peek(&num_read, bag);

    for (int i = 0; i < num_read; i++)
    {
        mino_draw(bag[i], 0, SECOND_X, SECOND_Y + (i + 0.5) * (MINO_H + gap_y), mino_get_default_colour(bag[i]), 1.0);
    }
}

void draw_held_piece(void)
{
    int offset_x = FIRST_X + 10;
    al_draw_textf(font, al_map_rgb_f(1, 1, 1), offset_x, SECOND_Y, 0, "Hold");

    PIECE held_piece = player_get_held_piece();

    if (held_piece >= 0 && held_piece < PIECE_MAX)
    {
        mino_draw(held_piece, 0, offset_x, SECOND_Y + 15, mino_get_default_colour(held_piece), 1.0);
    }
}

void draw_level(void)
{
    al_draw_textf(font, al_map_rgb_f(1, 1, 1), SECOND_X, FIRST_Y, 0, "Level %d", level_get());
}

void draw_remaining_lines(void)
{
    al_draw_textf(font, al_map_rgb_f(1, 1, 1), FIRST_X, BUFFER_H - 15, 0, "%d lines until next level", lines_until_next_level());
}

void draw_lines_cleared(void)
{
    al_draw_textf(font, al_map_rgb_f(1, 1, 1), FIRST_X, THIRD_Y, 0, "%d lines", lines_cleared_get());
}
