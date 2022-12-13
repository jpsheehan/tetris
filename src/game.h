#ifndef GAME_H_
#define GAME_H_

#include <allegro5/allegro5.h>

void game_init(void);
void game_deinit(void);
void game_update(ALLEGRO_EVENT* event, int frames);
void game_draw(void);

#endif