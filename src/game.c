#include <stdbool.h>
#include <stdio.h>
#include <allegro5/allegro_primitives.h>

#include "game.h"
#include "randomiser.h"
#include "player.h"
#include "field.h"
#include "score.h"
#include "hud.h"
#include "display.h"
#include "menu.h"
#include "utils.h"

void pause_menu_callback(int option);

static bool paused;
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
  paused = false;
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
}

void game_update(ALLEGRO_EVENT *event, int frames)
{
  if (!paused)
  {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
      paused = true;
    }
    else
    {
      player_update(event, frames);
      field_update();
    }
  }
  else
  {
    // handle pause menu events
    if (event->type == ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
      paused = false;
    }
    else
    {
      menu_update(&pause_menu, event);
    }
  }
  hud_update();
}

void game_draw()
{
  if (!paused)
  {
    field_draw(true);
    player_draw();
    hud_draw(true);
  }
  else
  {
    field_draw(false);
    hud_draw(false);

    // wash out the screen
    al_draw_filled_rectangle(0, 0, BUFFER_W, BUFFER_H, al_map_rgba_f(0, 0, 0, 0.5));

    // draw menu
    menu_draw(&pause_menu);
  }
}

void pause_menu_callback(int option)
{
  switch (option)
  {
  case 0:  // continue
  case -1: // cancel
    paused = false;
    pause_menu.idx = 0;
    break;
  case 1: // quit
    // TODO: signal proper exit
    safe_exit("Cancelled via menu", 0);
    break;
  }
}