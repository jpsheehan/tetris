#include <stdbool.h>
#include <stdio.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "game.h"
#include "randomiser.h"
#include "player.h"
#include "field.h"
#include "score.h"
#include "hud.h"
#include "display.h"
#include "menu.h"
#include "utils.h"
#include "audio.h"

void pause_menu_callback(int option);
void draw_preroll(void);

typedef enum GAME_STATE
{
  INIT,
  PREROLL,
  PLAYING,
  PAUSED
} GAME_STATE;

static ALLEGRO_TIMER *preroll = NULL;
static ALLEGRO_FONT *font = NULL;
static GAME_STATE state;
static MENU pause_menu = {
    .title = "Paused",
    .n_opts = 2,
    .x = BUFFER_W / 2,
    .y = 30,
    .opts = {
        "Continue",
        "Exit"},
    .idx = 0,
    .callback = &pause_menu_callback};

void game_init(void)
{
  state = INIT;
  preroll = al_create_timer(0.01);
  font = al_create_builtin_font();
  menu_init();
  randomiser_init();
  player_init();
  field_init();
  score_init();
  hud_init();
}

void game_deinit(void)
{
  player_deinit();
  randomiser_deinit();
  hud_deinit();

  if (preroll != NULL)
  {
    al_destroy_timer(preroll);
    preroll = NULL;
  }

  if (font != NULL)
  {
    al_destroy_font(font);
    font = NULL;
  }
}

void game_update(ALLEGRO_EVENT *event, int frames)
{

  switch (state)
  {
  case INIT:
    // enter PREROLL state
    al_set_timer_count(preroll, 0);
    al_start_timer(preroll);
    state = PREROLL;
    break;
  case PREROLL:
    // has countdown ended?
    int preroll_count = al_get_timer_count(preroll);
    if (preroll_count == 30)
    {
      audio_play_sfx(SFX_THREE);
    }
    else if (preroll_count == 130)
    {
      audio_play_sfx(SFX_TWO);
    }
    else if (preroll_count == 230)
    {
      audio_play_sfx(SFX_ONE);
    }
    else if (preroll_count == 330)
    {
      audio_play_sfx(SFX_GO);
    }
    else if (al_get_timer_count(preroll) >= 400)
    {
      state = PLAYING;
    }
    break;
  case PLAYING:
    if (event->type == ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
      state = PAUSED;
    }
    else
    {
      player_update(event, frames);
      field_update();
    }
    break;
  case PAUSED:
    if (event->type == ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
      state = INIT;
    }
    else
    {
      menu_update(&pause_menu, event);
    }
    break;
  }

  hud_update();
}

void game_draw()
{
  switch (state)
  {
  case INIT:
    field_draw(false);
    hud_draw(false);
    break;
  case PREROLL:
    field_draw(false);
    hud_draw(false);
    draw_preroll();
    break;
  case PLAYING:
    field_draw(true);
    player_draw();
    hud_draw(true);
    break;
  case PAUSED:
    field_draw(false);
    hud_draw(false);

    // wash out the screen
    al_draw_filled_rectangle(0, 0, BUFFER_W, BUFFER_H, al_map_rgba_f(0, 0, 0, 0.5));

    // draw menu
    menu_draw(&pause_menu);
    break;
  }
}

void pause_menu_callback(int option)
{
  switch (option)
  {
  case 0: // continue
    state = INIT;
    pause_menu.idx = 0;
    break;
  case 1: // quit
    // TODO: signal proper exit
    safe_exit("Cancelled via menu", 0);
    break;
  }
}

void draw_preroll(void)
{
  int countdown = 3 - al_get_timer_count(preroll) / 100;
  int t = al_get_timer_count(preroll) % 100;
  int y = t < 30 ? (t * BUFFER_H / 60) : (t < 70 ? BUFFER_H / 2 : (t - 40) * BUFFER_H / 60);

  if (countdown > 0)
    al_draw_textf(font, al_map_rgb_f(1, 1, 1), BUFFER_W / 2, y, ALLEGRO_ALIGN_CENTER, "%d!", countdown);
  else
    al_draw_text(font, al_map_rgb_f(1, 1, 1), BUFFER_W / 2, y, ALLEGRO_ALIGN_CENTER, "GO!");
}