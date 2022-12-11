#ifndef PLAYER_H_
#define PLAYER_H_

#include <allegro5/allegro.h>

#include "mino.h"

typedef struct PLAYER
{
    int x, y;
    int rotation;
    ALLEGRO_COLOR c;
    PIECE piece;
} PLAYER;

void player_init();
void player_deinit();
void player_update(ALLEGRO_EVENT *event, int frames);
void player_draw();

#endif