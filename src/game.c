#include <stdbool.h>
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
#include "asset_loader.h"

#define PREROLL_STEPS 60
#define PREROLL_RESOLUTION (1.0 / PREROLL_STEPS)

typedef enum GAME_STATE
{
  INIT,
  PREROLL,
  PLAYING,
  PAUSED,
  RETRY,
  WIN,
  DEFEAT,
} GAME_STATE;

typedef enum MODE
{
  MARATHON,
  SPRINT,
  ULTRA,
  ENDLESS,
} MODE;

static void win_menu_callback(int option);
static void pause_menu_callback(int option);
static void retry_menu_callback(int option);
static void draw_preroll(void);
static void player_callback(void);
static ALLEGRO_TIMER *create_preroll_timer(void);
static void reset_game_state(MODE newMode);
static void check_win_conditions(void);

static MODE mode;
static void (*callback)(void) = NULL;
static int sfx_played = 0b0000;
static ALLEGRO_TIMER *preroll = NULL;
static ALLEGRO_FONT *font = NULL;
static GAME_STATE state = INIT;
static MENU pause_menu = {
    .title = "Paused",
    .n_opts = 2,
    .x = BUFFER_W / 2,
    .y = 50,
    .opts = {
        "Continue",
        "Abandon"},
    .idx = 0,
    .callback = &pause_menu_callback,
};
static MENU retry_menu = {
    .title = "Retry?",
    .n_opts = 2,
    .x = BUFFER_W / 2,
    .y = 50,
    .opts = {
        "Yes",
        "No"},
    .idx = 0,
    .callback = &retry_menu_callback,
};
static MENU win_menu = {
    .title = "Win!",
    .n_opts = 1,
    .x = BUFFER_W / 2,
    .y = 50,
    .opts = {
        "OK",
    },
    .idx = 0,
    .callback = &win_menu_callback,
};

void game_init(void)
{
  if (preroll == NULL)
  {
    preroll = asset_loader_load(A_TIMER, (AssetLoaderCallback)&create_preroll_timer);
    must_init(preroll, "preroll timer");
  }

  if (font == NULL)
  {
    font = asset_loader_load(A_FONT, (AssetLoaderCallback)&al_create_builtin_font);
    must_init(font, "game font");
  }
}

void game_init_marathon(void (*cb)(void))
{
  callback = cb;
  game_init();
  reset_game_state(MARATHON);
}

void game_init_sprint(void (*cb)(void))
{
  callback = cb;
  game_init();
  reset_game_state(SPRINT);
}

void game_init_ultra(void (*cb)(void))
{
  callback = cb;
  game_init();
  reset_game_state(ULTRA);
}

void game_init_endless(void (*cb)(void))
{
  callback = cb;
  game_init();
  reset_game_state(ENDLESS);
}

void game_update(ALLEGRO_EVENT *pEvent, int frames)
{
  int preroll_count;

  switch (state)
  {
  case INIT:
    al_set_timer_count(preroll, 0);
    al_start_timer(preroll);
    state = PREROLL;
    sfx_played = 0b0000;
    break;
  case PREROLL:
    preroll_count = al_get_timer_count(preroll);
    if (preroll_count >= (int)(0.3 / PREROLL_RESOLUTION) && !(sfx_played & 0b0001))
    {
      audio_play_sfx(SFX_THREE);
      sfx_played |= 0b0001;
    }
    else if (preroll_count >= (int)(1.3 / PREROLL_RESOLUTION) && !(sfx_played & 0b0010))
    {
      audio_play_sfx(SFX_TWO);
      sfx_played |= 0b0010;
    }
    else if (preroll_count >= (int)(2.3 / PREROLL_RESOLUTION) && !(sfx_played & 0b0100))
    {
      audio_play_sfx(SFX_ONE);
      sfx_played |= 0b0100;
    }
    else if (preroll_count >= (int)(3.3 / PREROLL_RESOLUTION) && !(sfx_played & 0b1000))
    {
      audio_play_sfx(SFX_GO);
      sfx_played |= 0b1000;
    }
    else if (al_get_timer_count(preroll) >= (int)(4.0 / PREROLL_RESOLUTION))
    {
      al_stop_timer(preroll);
      state = PLAYING;
      audio_turn_music_up();
    }
    else if (pEvent->type == ALLEGRO_EVENT_KEY_DOWN && pEvent->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
      al_stop_timer(preroll);
      state = PAUSED;
      audio_turn_music_down();
    }
    break;
  case PLAYING:
    if (pEvent->type == ALLEGRO_EVENT_KEY_DOWN && pEvent->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
      state = PAUSED;
      audio_turn_music_down();
    }
    else
    {
      player_update(pEvent, frames);
      field_update();
      check_win_conditions();
    }
    break;
  case PAUSED:
    menu_update(&pause_menu, pEvent);
    break;
  case RETRY:
    menu_update(&retry_menu, pEvent);
    break;
  case WIN:
    menu_update(&win_menu, pEvent);
    break;
  default:
    safe_exit("Invalid game state", 1);
    break;
  }

  hud_update();
}

void game_draw(void)
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
    al_draw_filled_rectangle(0, 0, BUFFER_W, BUFFER_H, al_map_rgba_f(0, 0, 0, 0.5));
    menu_draw(&pause_menu);
    break;
  case RETRY:
    field_draw(true);
    hud_draw(true);
    al_draw_filled_rectangle(0, 0, BUFFER_W, BUFFER_H, al_map_rgba_f(0, 0, 0, 0.5));
    menu_draw(&retry_menu);
    break;
  case WIN:
    field_draw(true);
    hud_draw(true);
    al_draw_filled_rectangle(0, 0, BUFFER_W, BUFFER_H, al_map_rgba_f(0, 0, 0, 0.5));
    menu_draw(&win_menu);
    break;
  default:
    safe_exit("Invalid game state", 1);
    break;
  }
}

static void pause_menu_callback(int option)
{
  switch (option)
  {
  case -1:
  case 0: // continue
    state = INIT;
    pause_menu.idx = 0;
    break;
  case 1: // abandon game
    callback();
    break;
  default:
    safe_exit("Invalid pause menu option", 1);
    break;
  }
}

static void player_callback(void)
{
  state = RETRY;
}

static void retry_menu_callback(int option)
{
  switch (option)
  {
  case -1:
  case 1: // NO
    retry_menu.idx = 0;
    callback();
    break;
  case 0: // YES
    retry_menu.idx = 0;
    reset_game_state(mode);
    break;
  default:
    safe_exit("Invalid retry menu option", 1);
    break;
  }
}

static void win_menu_callback(int option)
{
  // TODO: Show initials entry menu instead
  switch (option)
  {
  case -1:
  case 0: // OK
    state = RETRY;
    break;
  default:
    safe_exit("Invalid menu index", 1);
    break;
  }
}

static ALLEGRO_TIMER *create_preroll_timer(void)
{
  return al_create_timer(PREROLL_RESOLUTION);
}

static void draw_preroll(void)
{
  int countdown = 3 - al_get_timer_count(preroll) / PREROLL_STEPS;
  int t = al_get_timer_count(preroll) % PREROLL_STEPS;
  int y = t < 0.3 * PREROLL_STEPS ? (t * BUFFER_H / (0.6 * PREROLL_STEPS)) : (t < (0.7 * PREROLL_STEPS) ? BUFFER_H / 2 : (t - 0.4 * PREROLL_STEPS) * BUFFER_H / (0.6 * PREROLL_STEPS));

  if (countdown > 0)
    al_draw_textf(font, al_map_rgb_f(1, 1, 1), BUFFER_W / 2, y, ALLEGRO_ALIGN_CENTER, "%d!", countdown);
  else
    al_draw_text(font, al_map_rgb_f(1, 1, 1), BUFFER_W / 2, y, ALLEGRO_ALIGN_CENTER, "GO!");
}

static void reset_game_state(MODE newMode)
{
  state = INIT;
  mode = newMode;

  randomiser_init();
  score_init();
  field_init();
  hud_init();
  player_init(&player_callback);
}

static void check_win_conditions(void)
{
  if (state != PLAYING)
    return;

  switch (mode)
  {
  case MARATHON:
    if (level_get() >= 15)
      state = WIN;
    break;
  case SPRINT:
    if (lines_cleared_get() >= 40)
      state = WIN;
    break;
  case ULTRA:
    // TODO: add check for ultra timer
    break;
  case ENDLESS:
    // no win conditions
    break;
  default:
    safe_exit("Invalid game state", 1);
    break;
  }
}