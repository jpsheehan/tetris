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
#include "input.h"
#include "transition.h"
#include "tetris.h"
#include "resources.h"
#include "leaderboard.h"

#define ASSERT_BONUS(bonus) ASSERT_RANGE(bonus, 0, BONUS_MAX, "bonus")

#define GAME_TRANSITION_S 0.5
#define PREROLL_STEPS 60
#define PREROLL_RESOLUTION (1.0 / PREROLL_STEPS)

typedef enum GAME_STATE
{
  TRANS_TO_INIT,
  INIT,
  PREROLL,
  PLAYING,
  PAUSED,
  RETRY,
  WIN,
  DEFEAT,
  CONFIRM,
  TRANS_TO_CALLBACK,
  TRANS_TO_RETRY_YES,
  TRANS_TO_RETRY_NO,
  TRANS_TO_RESTART,
} GAME_STATE;

static void win_menu_callback(int option);
static void pause_menu_callback(int option);
static void retry_menu_callback(int option);
static void confirm_menu_callback(int option);
static void show_confirm_restart(void);
static void show_confirm_abandon(void);
static void draw_preroll(void);
static void player_callback(void);
static void transition_callback(void);
static ALLEGRO_TIMER *create_preroll_timer(void);
static ALLEGRO_TIMER *create_ultra_timer(void);
static ALLEGRO_TIMER *create_bonus_timer(void);
static void reset_game_state(MODE newMode);
static void check_win_conditions(void);
static void enter_state_playing(void);
static void enter_state_paused(void);
#if !DISABLE_PREROLL
static void enter_state_preroll(void);
#endif
static ALLEGRO_FONT *create_preroll_font(void);

static void *transition = NULL;
static BONUS current_bonus = BONUS_MAX;
static int bonus_timer = 0;
static MODE mode;
static void (*callback)(void) = NULL;
static int sfx_played = 0b0000;
static int preroll = 0;
static int timer = 0;
static int preroll_font = 0;
static GAME_STATE state = INIT;
static char *CONFIRM_RESTART = "Really restart this game?";
static char *CONFIRM_ABANDON = "Really abandon this game?";
static char *WIN_NO_HIGHSCORE_TEXT = "You won, but no new high score";
static char *WIN_NEW_HIGHSCORE_TEXT = "You set a new high score!";
static MENU pause_menu = {
    .title = "Paused",
    .n_opts = 3,
    .x = BUFFER_W / 2,
    .y = 50,
    .opts = {
        "Continue",
        "Restart",
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
static MENU confirm_menu = {
    .title = NULL,
    .n_opts = 2,
    .x = BUFFER_W / 2,
    .y = 50,
    .opts = {
        "Yes",
        "No",
    },
    .idx = 1,
    .callback = &confirm_menu_callback,
};

static void game_init(void)
{
  if (preroll == 0)
  {
    preroll = asset_loader_load("preroll timer", A_TIMER, (AssetLoaderCallback)&create_preroll_timer);
  }

  if (timer == 0)
  {
    timer = asset_loader_load("ultra timer", A_TIMER, (AssetLoaderCallback)&create_ultra_timer);
  }

  if (bonus_timer == 0)
  {
    bonus_timer = asset_loader_load("bonus timer", A_TIMER, (AssetLoaderCallback)&create_bonus_timer);
  }

  if (preroll_font == 0)
  {
    preroll_font = asset_loader_load("preroll font", A_FONT, (AssetLoaderCallback)&create_preroll_font);
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

#if DEBUG_MENU
void game_init_debug(void (*cb)(void))
{
  callback = cb;
  game_init();

  state = INIT;
  mode = MARATHON;

  score_init();
  hud_init();
  player_init(&player_callback);
  al_set_timer_count(A(timer), 0);
}
#endif

void game_update(ALLEGRO_EVENT *pEvent, int frames)
{
  int preroll_count;

  switch (state)
  {
  case TRANS_TO_INIT:
  case TRANS_TO_CALLBACK:
  case TRANS_TO_RETRY_YES:
  case TRANS_TO_RETRY_NO:
  case TRANS_TO_RESTART:
    transition_update(transition, pEvent);
    break;
  case INIT:
#if DISABLE_PREROLL
    enter_state_playing();
#else
    enter_state_preroll();
#endif
    break;
  case PREROLL:
    preroll_count = al_get_timer_count(A(preroll));
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
    else if (al_get_timer_count(A(preroll)) >= (int)(4.0 / PREROLL_RESOLUTION))
    {
      enter_state_playing();
    }
    else if (pEvent->type == ALLEGRO_EVENT_KEY_DOWN && pEvent->keyboard.keycode == input_get_mapping(INPUT_PAUSE))
    {
      enter_state_paused();
    }
    break;
  case PLAYING:
    if (pEvent->type == ALLEGRO_EVENT_KEY_DOWN && pEvent->keyboard.keycode == input_get_mapping(INPUT_PAUSE))
    {
      enter_state_paused();
    }
    else
    {
      player_update(pEvent, frames);
      field_update(); // must be run after player_update!
      check_win_conditions();
    }
    break;
  case PAUSED:
    menu_update(&pause_menu, pEvent);
    break;
  case RETRY:
    al_stop_timer(A(timer));
    al_stop_timer(A(bonus_timer));
    menu_update(&retry_menu, pEvent);
    break;
  case WIN:
    al_stop_timer(A(timer));
    al_stop_timer(A(bonus_timer));
    menu_update(&win_menu, pEvent);
    break;
  case CONFIRM:
    menu_update(&confirm_menu, pEvent);
    break;
  default:
    safe_exit("Invalid game state", 1);
    break;
  }

  if (current_bonus != BONUS_MAX && al_get_timer_count(A(bonus_timer)) >= 1 && al_get_timer_started(A(bonus_timer)))
  {
    al_stop_timer(A(bonus_timer));
    current_bonus = BONUS_MAX;
  }

  hud_update();
}

void game_draw(void)
{
  HUD_UPDATE_DATA data = {
      .mode = mode,
      .timer_count = timer == 0 ? 0 : (int)al_get_timer_count(A(timer)),
      .show_minos = state == PLAYING || state == RETRY || state == WIN || state == TRANS_TO_RETRY_YES || state == TRANS_TO_RETRY_NO,
      .timer_running = al_get_timer_started(A(timer)),
      .bonus = current_bonus,
  };

  switch (state)
  {
  case TRANS_TO_INIT:
    field_draw(data.show_minos);
    hud_draw(&data);
    transition_draw(transition);
    break;
  case TRANS_TO_CALLBACK:
  case TRANS_TO_RESTART:
    field_draw(data.show_minos);
    hud_draw(&data);
    al_draw_filled_rectangle(0, 0, BUFFER_W, BUFFER_H, al_map_rgba_f(0, 0, 0, 0.5));
    menu_draw(&confirm_menu);
    transition_draw(transition);
    break;
  case TRANS_TO_RETRY_YES:
  case TRANS_TO_RETRY_NO:
    field_draw(data.show_minos);
    hud_draw(&data);
    al_draw_filled_rectangle(0, 0, BUFFER_W, BUFFER_H, al_map_rgba_f(0, 0, 0, 0.5));
    menu_draw(&retry_menu);
    transition_draw(transition);
    break;
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
  case CONFIRM:
    field_draw(data.show_minos);
    hud_draw(&data);
    al_draw_filled_rectangle(0, 0, BUFFER_W, BUFFER_H, al_map_rgba_f(0, 0, 0, 0.5));
    menu_draw(&confirm_menu);
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
  case TSPIN_MINI_SINGLE_NO_LINES:
  case TSPIN_MINI_SINGLE:
    audio_play_sfx(SFX_TSPIN_MINI);
    break;
  case TSPIN_SINGLE_NO_LINES:
  case TSPIN_SINGLE:
  case TSPIN_DOUBLE:
  case TSPIN_TRIPLE:
    audio_play_sfx(SFX_TSPIN_PROPER);
    break;
  default:
    break;
  }

  al_stop_timer(A(bonus_timer));
  al_set_timer_count(A(bonus_timer), 0);
  al_start_timer(A(bonus_timer));
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
  case 1:
    show_confirm_restart();
    break;
  case 2:
    show_confirm_abandon();
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
    state = TRANS_TO_RETRY_NO;
    if (transition != NULL)
      transition_free(transition);
    transition = transition_start(FADE_OUT, GAME_TRANSITION_S, &transition_callback);
    break;
  case 0: // YES
    state = TRANS_TO_RETRY_YES;
    if (transition != NULL)
      transition_free(transition);
    transition = transition_start(FADE_OUT, GAME_TRANSITION_S, &transition_callback);
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
  return al_create_timer(0.001);
}

static ALLEGRO_TIMER *create_bonus_timer(void)
{
  return al_create_timer(2.0);
}

static void draw_preroll(void)
{
  int countdown = 3 - al_get_timer_count(A(preroll)) / PREROLL_STEPS;
  int t = al_get_timer_count(A(preroll)) % PREROLL_STEPS;
  int middle = BUFFER_H / 2 - al_get_font_line_height(A(preroll_font));
  int y = t < 0.3 * PREROLL_STEPS
        ? (t * middle / (0.3 * PREROLL_STEPS))
        : (t < (0.7 * PREROLL_STEPS)
          ? middle
          : (t - 0.4 * PREROLL_STEPS) * middle / (0.3 * PREROLL_STEPS));

  if (countdown > 0)
    al_draw_textf(A(preroll_font), al_map_rgb_f(1, 1, 1), BUFFER_W / 2, y, ALLEGRO_ALIGN_CENTER, "%d!", countdown);
  else
    al_draw_text(A(preroll_font), al_map_rgb_f(1, 1, 1), BUFFER_W / 2, y, ALLEGRO_ALIGN_CENTER, "GO!");
}

static void reset_game_state(MODE newMode)
{
  state = TRANS_TO_INIT;
  mode = newMode;
  current_bonus = BONUS_MAX;

  randomiser_init();

#if DEBUG_MENU && MAKE_LOGO
  PIECE piece_buffer[4] = {T, I, T, L};
  randomiser_seed(4, piece_buffer);
#endif

  score_init();
  field_init();
  hud_init();
  player_init(&player_callback);
  al_set_timer_count(A(timer), 0);

  state = TRANS_TO_INIT;
  if (transition != NULL)
    transition_free(transition);
  transition = transition_start(FADE_IN, GAME_TRANSITION_S, &transition_callback);

#if DEBUG_MENU && MAKE_LOGO
  player_make_logo();
#endif
}

static void check_win_conditions(void)
{
  if (state != PLAYING)
    return;

  switch (mode)
  {
  case MARATHON:
    if (level_get() > LAST_MARATHON_LEVEL)
    {
      state = WIN;
      if (leaderboard_is_worthy(MARATHON, score_get())) {
        leaderboard_add_score(MARATHON, "TEST", score_get());
        win_menu.title = WIN_NEW_HIGHSCORE_TEXT;
      } else {
        win_menu.title = WIN_NO_HIGHSCORE_TEXT;
      }
    }
    break;
  case SPRINT:
    if (lines_cleared_get() >= MAX_SPRINT_LINES) {
      state = WIN;
      if (leaderboard_is_worthy(SPRINT, score_get()))
      {
        leaderboard_add_score(SPRINT, "TEST", score_get());
        win_menu.title = WIN_NEW_HIGHSCORE_TEXT;
      } else {
        win_menu.title = WIN_NO_HIGHSCORE_TEXT;
      }
    }
    break;
  case ULTRA:
    if (al_get_timer_count(A(timer)) >= MAX_ULTRA_SECONDS * 1000) {
      state = WIN;
      if (leaderboard_is_worthy(ULTRA, score_get())) {
        leaderboard_add_score(ULTRA, "TEST", score_get());
        win_menu.title = WIN_NEW_HIGHSCORE_TEXT;
      } else {
        win_menu.title = WIN_NO_HIGHSCORE_TEXT;
      }
    }
    break;
  case ENDLESS:
    // no win conditions
    break;
  default:
    safe_exit("Invalid game state", 1);
    break;
  }
}

#if !DISABLE_PREROLL
static void enter_state_preroll(void)
{
  al_set_timer_count(A(preroll), 0);
  al_start_timer(A(preroll));
  state = PREROLL;
  sfx_played = 0b0000;
}
#endif

static void enter_state_playing(void)
{
  al_stop_timer(A(preroll));
  state = PLAYING;
  audio_turn_music_up();
  al_start_timer(A(timer));
  if (current_bonus != BONUS_MAX)
  {
    al_start_timer(A(bonus_timer));
  }
}

static void enter_state_paused(void)
{
  al_stop_timer(A(preroll));
  al_stop_timer(A(timer));
  al_stop_timer(A(bonus_timer));
  state = PAUSED;
  audio_turn_music_down();
}

static void confirm_menu_callback(int option)
{
  switch (option)
  {
  case 0: // YES
    if (confirm_menu.title == CONFIRM_RESTART)
    {
      state = TRANS_TO_RESTART;
      if (transition != NULL)
        transition_free(transition);
      transition = transition_start(FADE_OUT, GAME_TRANSITION_S, &transition_callback);
    }
    else if (confirm_menu.title == CONFIRM_ABANDON)
    {
      al_stop_timer(A(bonus_timer));
      current_bonus = BONUS_MAX;
      pause_menu.idx = 0;
      state = TRANS_TO_CALLBACK;
      if (transition != NULL)
        transition_free(transition);
      transition = transition_start(FADE_OUT, GAME_TRANSITION_S, &transition_callback);
    }
    else
    {
      safe_exit("Invalid confirm_menu.title", 1);
    }
    break;
  case 1:
  case -1: // NO
    state = PAUSED;
    break;
  default:
    safe_exit("Invalid confirm menu callback index", 1);
    break;
  }
}

static void show_confirm_restart(void)
{
  confirm_menu.idx = 1;
  confirm_menu.title = CONFIRM_RESTART;
  state = CONFIRM;
}

static void show_confirm_abandon(void)
{
  confirm_menu.idx = 1;
  confirm_menu.title = CONFIRM_ABANDON;
  state = CONFIRM;
}

static void transition_callback(void)
{
  transition_free(transition);
  transition = NULL;

  switch (state)
  {
  case TRANS_TO_INIT:
    state = INIT;
    break;
  case TRANS_TO_RETRY_NO:
    retry_menu.idx = 0;
  case TRANS_TO_CALLBACK:
    state = TRANS_TO_INIT;
    callback();
    break;
  case TRANS_TO_RESTART:
    game_init();
    reset_game_state(mode);
    break;
  case TRANS_TO_RETRY_YES:
    reset_game_state(mode); // sets state and creates new transition
    break;
  default:
    safe_exit("Invalid state for transition callback", 1);
    break;
  }
}

static ALLEGRO_FONT *create_preroll_font(void)
{
  return al_load_font(R_FONT_XOLONIUM_REGULAR, REL(2), 0);
}