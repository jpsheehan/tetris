#include "game.h"
#include "randomiser.h"
#include "player.h"
#include "field.h"
#include "score.h"
#include "hud.h"

void game_init(void)
{
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

void game_update(ALLEGRO_EVENT* event, int frames)
{
  player_update(event, frames);
  field_update();
  hud_update();
}

void game_draw()
{
  field_draw();
  player_draw();
  hud_draw();
}