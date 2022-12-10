#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "utils.h"
#include "score.h"

static ALLEGRO_FONT* font = NULL;
static long score = 0;
static long score_display = 0;

void score_init(void)
{
    font = al_create_builtin_font();
}

void score_deinit(void)
{
    if (font != NULL)
    {
        al_destroy_font(font);
        font = NULL;
    }
}

void score_increase(int delta)
{
    score += delta;
}

void score_update(int frames)
{
    if (score != score_display && frames % (FPS / 10) == 0) // 10 times per second
    {
        score_display += 25;
    }
}

void score_draw(void)
{
    al_draw_textf(font, al_map_rgb_f(1, 1, 1), 10, 10, 0, "%06ld", score_display);
}