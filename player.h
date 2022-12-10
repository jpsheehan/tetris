#ifndef PLAYER_H_
#define PLAYER_H_

#include <allegro5/allegro.h>

typedef enum PIECE
{
    O,
    PIECE_MAX
} PIECE;

typedef struct PLAYER
{
    int x, y;
    ALLEGRO_COLOR c;
    PIECE piece;
} PLAYER;

void player_init();
void player_update(ALLEGRO_EVENT *event, int frames);
void player_draw();

#endif