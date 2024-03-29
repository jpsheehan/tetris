#ifndef PLAYER_H_
#define PLAYER_H_

#include <allegro5/allegro.h>

#include "tetris.h"
#include "mino.h"

#define CLEAR_TIMER_RESOLUTION 100.0

typedef struct PLAYER
{
    int x, y;
    int rotation;
    ALLEGRO_COLOR c;
    PIECE piece;
} PLAYER;

typedef enum TSPIN
{
    TS_NONE,
    TS_MINI,
    TS_PROPER,
} TSPIN;

void player_init(void(*cb)(void));
void player_update(ALLEGRO_EVENT *event, int frames);
void player_draw();
PIECE player_get_held_piece(void);
TSPIN player_get_tspin_state(void);
void player_pause_timers(void);
void player_start_timers(void);
long player_get_clear_timer_count(void);

#if MAKE_LOGO
void player_make_logo(void);
#endif

#endif