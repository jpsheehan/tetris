#ifndef TRANSITION_H_
#define TRANSITION_H_

#include <allegro5/allegro.h>

typedef enum TRANSITION_TYPE {
    NONE,
    FADE_IN,
    FADE_OUT,
} TRANSITION_TYPE;

void transition_init(void);

void transition_draw(void* pTransition);
void transition_update(void* pTransition, ALLEGRO_EVENT* pEvent);
void* transition_start(TRANSITION_TYPE type, float time_s, void(*cb)(void));
void transition_free(void* pTransition);

#endif