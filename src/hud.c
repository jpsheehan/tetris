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
#include "resources.h"

#define LEFT REL(0.5) // left
#define TOP REL(0.5)  // top

#define RIGHT (BUFFER_W - REL(0.5))  // right
#define BOTTOM (BUFFER_H - REL(1.5)) // bottom

#define CENTER (BUFFER_W / 2)            // center
#define MIDDLE (BUFFER_H / 2 - REL(0.5)) // middle

static int score_font = 0;
static int flash_timer = 0;
static int level_font = 0;
static int remaining_lines_font = 0;
static int bonus_font = 0;
static int time_font = 0;
static int next_font = 0;
static int hold_font = 0;
static int lines_cleared_font = 0;

static ALLEGRO_FONT *load_score_font(void);
static ALLEGRO_FONT *load_level_font(void);
static ALLEGRO_FONT *load_remaining_lines_font(void);
static ALLEGRO_FONT *load_bonus_font(void);
static ALLEGRO_FONT *load_time_font(void);
static ALLEGRO_FONT *load_next_font(void);
static ALLEGRO_FONT *load_hold_font(void);
static ALLEGRO_FONT *load_lines_cleared_font(void);

static void draw_score(void);
static void draw_randomiser(bool show_minos);
static void draw_held_piece(bool show_minos);
static void draw_level(void);
static void draw_remaining_lines(void);
static void draw_lines_cleared(void);
static ALLEGRO_TIMER *create_flash_timer(void);

static long score_display;

void hud_init(void)
{
    if (score_font == 0)
    {
        score_font = asset_loader_load("score font", A_FONT, (AssetLoaderCallback)&load_score_font);
    }

    if (level_font == 0)
    {
        level_font = asset_loader_load("level font", A_FONT, (AssetLoaderCallback)&load_level_font);
    }

    if (remaining_lines_font == 0)
    {
        remaining_lines_font = asset_loader_load("remaining lines font", A_FONT, (AssetLoaderCallback)&load_remaining_lines_font);
    }

    if (bonus_font == 0)
    {
        bonus_font = asset_loader_load("bonus font", A_FONT, (AssetLoaderCallback)&load_bonus_font);
    }

    if (time_font == 0)
    {
        time_font = asset_loader_load("time font", A_FONT, (AssetLoaderCallback)&load_time_font);
    }

    if (hold_font == 0)
    {
        hold_font = asset_loader_load("hold font", A_FONT, (AssetLoaderCallback)&load_hold_font);
    }

    if (next_font == 0)
    {
        next_font = asset_loader_load("next font", A_FONT, (AssetLoaderCallback)&load_next_font);
    }

    if (lines_cleared_font == 0)
    {
        lines_cleared_font = asset_loader_load("lines_cleared font", A_FONT, (AssetLoaderCallback)&load_lines_cleared_font);
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

static char *bonus_words[] = {
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
    draw_randomiser(pData->show_minos);
    draw_held_piece(pData->show_minos);
    draw_level();
    draw_remaining_lines();
    draw_lines_cleared();

    if (pData->timer_running || (!pData->timer_running && al_get_timer_count(A(flash_timer)) % 2 == 0))
    {
        int milliseconds_to_show = pData->mode == ULTRA ? (MAX_ULTRA_SECONDS * 1000) - pData->timer_count : pData->timer_count;

        int minutes = milliseconds_to_show / 60000;
        minutes = CLAMP(minutes, 0, 99);

        int seconds = (milliseconds_to_show / 1000) % 60;
        seconds = CLAMP(seconds, 0, 59);

        int milliseconds = milliseconds_to_show % 1000;
        milliseconds = CLAMP(milliseconds, 0, 999);

        al_draw_textf(A(time_font), al_map_rgb_f(1, 1, 1), LEFT, MIDDLE - REL(2), ALLEGRO_ALIGN_LEFT, "%02d:%02d.%03d", minutes, seconds, milliseconds);
    }

    if (pData->bonus >= 0 && pData->bonus < BONUS_MAX)
    {
        char *bonus_text = bonus_words[pData->bonus];
        al_draw_textf(A(bonus_font), al_map_rgb_f(1, 1, 1), LEFT, MIDDLE, ALLEGRO_ALIGN_LEFT, "%s!", bonus_text);
    }
}

static void draw_score(void)
{
    al_draw_textf(A(score_font), al_map_rgb_f(1, 1, 1), LEFT, TOP, ALLEGRO_ALIGN_LEFT, "%06ld", score_display);
}

static void draw_randomiser(bool show_minos)
{
    al_draw_text(A(next_font), al_map_rgb_f(1, 1, 1), RIGHT, TOP + REL(1.5), ALLEGRO_ALIGN_RIGHT, "Next");
    
    if (!show_minos) return;

    int gap_y = MINO_H * 2;
    PIECE bag[6];
    int num_read = 0;

    randomiser_peek(&num_read, bag);

    for (int i = 0; i < num_read; i++)
    {
        mino_draw(bag[i], 0, RIGHT - MINO_H * 4, TOP + REL(1.5) + (i + 1) * (MINO_H + gap_y), mino_get_default_colour(bag[i]), 1.0);
    }
}

static void draw_held_piece(bool show_minos)
{
    al_draw_textf(A(hold_font), al_map_rgb_f(1, 1, 1), LEFT, TOP + REL(1.5), 0, "Hold");

    if (!show_minos) return;

    PIECE held_piece = player_get_held_piece();

    if (held_piece >= 0 && held_piece < PIECE_MAX)
    {
        mino_draw(held_piece, 0, LEFT, TOP + REL(3), mino_get_default_colour(held_piece), 1.0);
    }
}

static void draw_level(void)
{
    al_draw_textf(A(level_font), al_map_rgb_f(1, 1, 1), RIGHT, TOP, ALLEGRO_ALIGN_RIGHT, "Level %d", level_get());
}

static void draw_remaining_lines(void)
{
    al_draw_textf(A(remaining_lines_font), al_map_rgb_f(1, 1, 1), LEFT, BOTTOM, 0, "%d lines until next level", lines_until_next_level());
}

static void draw_lines_cleared(void)
{
    al_draw_textf(A(lines_cleared_font), al_map_rgb_f(1, 1, 1), LEFT, MIDDLE + REL(2), 0, "%d lines", lines_cleared_get());
}

static ALLEGRO_TIMER *create_flash_timer(void)
{
    return al_create_timer(0.5);
}

static ALLEGRO_FONT *load_score_font(void)
{
    return al_load_font(R_FONT_XOLONIUM_REGULAR, REL(1), 0);
}

static ALLEGRO_FONT *load_level_font(void)
{
    return al_load_font(R_FONT_XOLONIUM_REGULAR, REL(1), 0);
}

static ALLEGRO_FONT *load_remaining_lines_font(void)
{
    return al_load_font(R_FONT_XOLONIUM_REGULAR, REL(1), 0);
}

static ALLEGRO_FONT *load_time_font(void)
{
    return al_load_font(R_FONT_XOLONIUM_REGULAR, REL(1), 0);
}

static ALLEGRO_FONT *load_hold_font(void)
{
    return al_load_font(R_FONT_XOLONIUM_REGULAR, REL(1), 0);
}

static ALLEGRO_FONT *load_next_font(void)
{
    return al_load_font(R_FONT_XOLONIUM_REGULAR, REL(1), 0);
}

static ALLEGRO_FONT *load_bonus_font(void)
{
    return al_load_font(R_FONT_XOLONIUM_REGULAR, REL(1), 0);
}

static ALLEGRO_FONT *load_lines_cleared_font(void)
{
    return al_load_font(R_FONT_XOLONIUM_REGULAR, REL(1), 0);
}
