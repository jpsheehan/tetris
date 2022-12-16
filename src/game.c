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

#define ASSERT_BONUS(bonus) ASSERT_RANGE(bonus, 0, BONUS_MAX, "bonus")

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

static void win_menu_callback(int option);
static void pause_menu_callback(int option);
static void retry_menu_callback(int option);
static void draw_preroll(void);
static void player_callback(void);
static ALLEGRO_TIMER *create_preroll_timer(void);
static ALLEGRO_TIMER *create_ultra_timer(void);
static ALLEGRO_TIMER *create_bonus_timer(void);
static void reset_game_state(MODE newMode);
static void check_win_conditions(void);

static BONUS current_bonus = BONUS_MAX;
static ALLEGRO_TIMER *bonus_timer = NULL;
static MODE mode;
static void (*callback)(void) = NULL;
static int sfx_played = 0b0000;
static ALLEGRO_TIMER *preroll = NULL;
static ALLEGRO_TIMER *timer = NULL;
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

static void game_init(void)
{
  if (preroll == NULL)
  {
    preroll = asset_loader_load(A_TIMER, (AssetLoaderCallback)&create_preroll_timer);
    must_init(preroll, "preroll timer");
  }

  if (timer == NULL)
  {
    timer = asset_loader_load(A_TIMER, (AssetLoaderCallback)&create_ultra_timer);
    must_init(timer, "ultra timer");
  }

  if (bonus_timer == NULL)
  {
    bonus_timer = asset_loader_load(A_TIMER, (AssetLoaderCallback)&create_bonus_timer);
    must_init(timer, "bonus timer");
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
      al_start_timer(timer);
      if (current_bonus != BONUS_MAX)
      {
        al_start_timer(bonus_timer);
      }
    }
    else if (pEvent->type == ALLEGRO_EVENT_KEY_DOWN && pEvent->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
      al_stop_timer(preroll);
      al_stop_timer(timer);
      al_stop_timer(bonus_timer);
      state = PAUSED;
      audio_turn_music_down();
    }
    break;
  case PLAYING:
    if (pEvent->type == ALLEGRO_EVENT_KEY_DOWN && pEvent->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
      al_stop_timer(timer);
      al_stop_timer(bonus_timer);
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
    al_stop_timer(timer);
    al_stop_timer(bonus_timer);
    menu_update(&retry_menu, pEvent);
    break;
  case WIN:
    al_stop_timer(timer);
    al_stop_timer(bonus_timer);
    menu_update(&win_menu, pEvent);
    break;
  default:
    safe_exit("Invalid game state", 1);
    break;
  }

  if (current_bonus != BONUS_MAX && al_get_timer_count(bonus_timer) >= 1 && al_get_timer_started(bonus_timer))
  {
    al_stop_timer(bonus_timer);
    current_bonus = BONUS_MAX;
  }

  hud_update();
}

void game_draw(void)
{
  HUD_UPDATE_DATA data = {
      .mode = mode,
      .timer_count = timer == NULL ? 0 : (int)al_get_timer_count(timer),
      .show_minos = state == PLAYING || state == RETRY || state == WIN,
      .timer_running = al_get_timer_started(timer),
      .bonus = current_bonus,
  };

  switch (state)
  {
  case INIT:
    field_draw(data.show_minos);
    hud_draw(&data);
    break;
  case PREROLL:
    field_draw(data.show_minos);
    hud_draw(&data);
    draw_preroll();
    break;
  case PLAYING:
    field_draw(data.show_minos);
    player_draw();
    hud_draw(&data);
    break;
  case PAUSED:
    field_draw(data.show_minos);
    hud_draw(&data);
    al_draw_filled_rectangle(0, 0, BUFFER_W, BUFFER_H, al_map_rgba_f(0, 0, 0, 0.5));
    menu_draw(&pause_menu);
    break;
  case RETRY:
    field_draw(data.show_minos);
    hud_draw(&data);
    al_draw_filled_rectangle(0, 0, BUFFER_W, BUFFER_H, al_map_rgba_f(0, 0, 0, 0.5));
    menu_draw(&retry_menu);
    break;
  case WIN:
    field_draw(data.show_minos);
    hud_draw(&data);
    al_draw_filled_rectangle(0, 0, BUFFER_W, BUFFER_H, al_map_rgba_f(0, 0, 0, 0.5));
    menu_draw(&win_menu);
    break;
  default:
    safe_exit("Invalid game state", 1);
    break;
  }
}

void game_show_bonus(BONUS bonus)
{
  ASSERT_BONUS(bonus);

  score_add(bonus);
  current_bonus = bonus;

  switch (bonus)
  {
  case SINGLE:
  case DOUBLE:
  case TRIPLE:
  case TETRIS:
    audio_play_sfx(SFX_LINE_CLEAR);
    break;
  case PERFECT_CLEAR_SINGLE:
  case PERFECT_CLEAR_DOUBLE:
  case PERFECT_CLEAR_TRIPLE:
  case PERFECT_CLEAR_TETRIS:
    audio_play_sfx(SFX_PERFECT_CLEAR);
    break;
  default:
    break;
  }

  al_stop_timer(bonus_timer);
  al_set_timer_count(bonus_timer, 0);
  al_start_timer(bonus_timer);
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

static ALLEGRO_TIMER *create_ultra_timer(void)
{
  return al_create_timer(1.0);
}

static ALLEGRO_TIMER *create_bonus_timer(void)
{
  return al_create_timer(2.0);
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
  al_set_timer_count(timer, 0);
}

static void check_win_conditions(void)
{
  if (state != PLAYING)
    return;

  switch (mode)
  {
  case MARATHON:
    if (level_get() >= MAX_MARATHON_LEVEL)
      state = WIN;
    break;
  case SPRINT:
    if (lines_cleared_get() >= MAX_SPRINT_LINES)
      state = WIN;
    break;
  case ULTRA:
    if (al_get_timer_count(timer) >= MAX_ULTRA_SECONDS)
      state = WIN;
    break;
  case ENDLESS:
    // no win conditions
    break;
  default:
    safe_exit("Invalid game state", 1);
    break;
  }
}