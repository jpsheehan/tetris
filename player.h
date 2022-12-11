#ifndef PLAYER_H_
#define PLAYER_H_

#include <allegro5/allegro.h>

typedef enum PIECE
{
    O,
    T,
    I,
    J,
    L,
    S,
    Z,
    PIECE_MAX
} PIECE;

typedef struct PLAYER
{
    int x, y;
    int rotation;
    ALLEGRO_COLOR c;
    PIECE piece;
} PLAYER;

void player_init();
void player_update(ALLEGRO_EVENT *event, int frames);
void player_draw();

#endif