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

static bool paused;

void game_init(void)
{
  paused = false;
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
      printf("Paused\n");
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
    // TODO
  }
}