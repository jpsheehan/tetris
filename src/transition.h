#ifndef TRANSITION_H_
#define TRANSITION_H_

#include <allegro5/allegro.h>

typedef enum TRANSITION {
    NONE,
    FADE_IN,
    FADE_OUT,
} TRANSITION;

void transition_init(void);
void transition_draw(void);
void transition_update(ALLEGRO_EVENT* pEvent);

void transition_start(TRANSITION transition, float time_s, void(*cb)(void));

#endif