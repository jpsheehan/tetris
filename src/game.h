#ifndef GAME_H_
#define GAME_H_

#include <allegro5/allegro5.h>

#define MAX_MARATHON_LEVEL 15
#define MAX_SPRINT_LINES 40
#define MAX_ULTRA_SECONDS 120

typedef enum MODE
{
    MARATHON,
    SPRINT,
    ULTRA,
    ENDLESS,
} MODE;

typedef enum BONUS
{
    SINGLE,
    DOUBLE,
    TRIPLE,
    TETRIS,
    //   TSPIN,
    //   TSPIN_MINI,
    //   TSPIN_SINGLE,
    //   TSPIN_MINI_SINGLE,
    //   TSPIN_DOUBLE,
    //   TSPIN_MINI_DOUBLE,
    //   TSPIN_TRIPLE,
    //   TSPIN_MINI_TRIPLE,
    PERFECT_CLEAR_SINGLE,
    PERFECT_CLEAR_DOUBLE,
    PERFECT_CLEAR_TRIPLE,
    PERFECT_CLEAR_TETRIS,
    BONUS_MAX,
} BONUS;

void game_init_marathon(void (*cb)(void));
void game_init_sprint(void (*cb)(void));
void game_init_ultra(void (*cb)(void));
void game_init_endless(void (*cb)(void));
void game_update(ALLEGRO_EVENT *event, int frames);
void game_draw(void);
void game_show_bonus(BONUS bonus);

#endif