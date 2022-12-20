#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "hud.h"
#include "score.h"
#include "display.h"
#include "mino.h"
#include "randomiser.h"
#include "player.h"
#include "utils.h"
#include "asset_loader.h"

#define FIRST_X 10
#define FIRST_Y 10

#define SECOND_X (BUFFER_W - 70)
#define SECOND_Y 40

#define THIRD_Y (BUFFER_H - 70)

static int font = 0;
static int flash_timer = 0;
static long score_display;

static void draw_score(void);
static void draw_randomiser(void);
static void draw_held_piece(void);
static void draw_level(void);
static void draw_remaining_lines(void);
static void draw_lines_cleared(void);
static ALLEGRO_TIMER *create_flash_timer(void);

void hud_init(void)
{
    if (font == 0)
    {
        font = asset_loader_load("hud font", A_FONT, (AssetLoaderCallback)&al_create_builtin_font);
    }

    if (flash_timer == 0)
    {
        flash_timer = asset_loader_load("flash timer", A_TIMER, (AssetLoaderCallback)&create_flash_timer);
    }

    score_display = 0;
    al_set_timer_count(A(flash_timer), 0);
    al_start_timer(A(flash_timer));
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

static char* bonus_words[] = {
    "Single",
    "Double",
    "Triple",
    "Tetris",
    "T-Spin",
    "T-Spin",
    "T-Spin Mini",
    "T-Spin Mini",
    "T-Spin Double",
    "T-Spin Triple",
    "Perfect Clear",
    "Perfect Clear Double",
    "Perfect Clear Triple",
    "Perfect Clear Tetris",
};

void hud_draw(HUD_UPDATE_DATA *pData)
{
    draw_score();
    if (pData->show_minos)
    {
        draw_randomiser();
        draw_held_piece();
    }
    draw_level();
    draw_remaining_lines();
    draw_lines_cleared();

    if (pData->timer_running || (!pData->timer_running && al_get_timer_count(A(flash_timer)) % 2 == 0))
    {
        int seconds_to_show = pData->mode == ULTRA ? MAX_ULTRA_SECONDS - pData->timer_count : pData->timer_count;
        al_draw_textf(A(font), al_map_rgb_f(1, 1, 1), 30, 90, 0, "%02d:%02d", seconds_to_show / 60, seconds_to_show % 60);
    }

    if (pData->bonus >= 0 && pData->bonus < BONUS_MAX)
    {
        char* bonus_text = bonus_words[pData->bonus];
        al_draw_textf(A(font), al_map_rgb_f(1, 1, 1), 30, 110, 0, "%s!", bonus_text);
    }
}

static void draw_score(void)
{
    al_draw_textf(A(font), al_map_rgb_f(1, 1, 1), 10, FIRST_Y, 0, "%06ld", score_display);
}

static void draw_randomiser(void)
{
    al_draw_text(A(font), al_map_rgb_f(1, 1, 1), SECOND_X, SECOND_Y, 0, "Next");

    int gap_y = 20;
    PIECE bag[6];
    int num_read = 0;

    randomiser_peek(&num_read, bag);

    for (int i = 0; i < num_read; i++)
    {
        mino_draw(bag[i], 0, SECOND_X, SECOND_Y + (i + 0.5) * (MINO_H + gap_y), mino_get_default_colour(bag[i]), 1.0);
    }
}

static void draw_held_piece(void)
{
    int offset_x = FIRST_X + 10;
    al_draw_textf(A(font), al_map_rgb_f(1, 1, 1), offset_x, SECOND_Y, 0, "Hold");

    PIECE held_piece = player_get_held_piece();

    if (held_piece >= 0 && held_piece < PIECE_MAX)
    {
        mino_draw(held_piece, 0, offset_x, SECOND_Y + 15, mino_get_default_colour(held_piece), 1.0);
    }
}

static void draw_level(void)
{
    al_draw_textf(A(font), al_map_rgb_f(1, 1, 1), SECOND_X, FIRST_Y, 0, "Level %d", level_get());
}

static void draw_remaining_lines(void)
{
    al_draw_textf(A(font), al_map_rgb_f(1, 1, 1), FIRST_X, BUFFER_H - 15, 0, "%d lines until next level", lines_until_next_level());
}

static void draw_lines_cleared(void)
{
    al_draw_textf(A(font), al_map_rgb_f(1, 1, 1), FIRST_X, THIRD_Y, 0, "%d lines", lines_cleared_get());
}

static ALLEGRO_TIMER *create_flash_timer(void)
{
    return al_create_timer(0.5);
}