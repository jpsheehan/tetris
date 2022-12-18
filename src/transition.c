#include <allegro5/allegro_primitives.h>
#include <stdio.h>

#include "transition.h"
#include "asset_loader.h"
#include "utils.h"
#include "display.h"

#define TRANSITION_STEPS 60.0

static ALLEGRO_TIMER *create_timer(void);

typedef struct TRANSITION
{
    TRANSITION_TYPE type;
    ALLEGRO_TIMER *timer;
    void (*callback)(void);
} TRANSITION;

void *transition_start(TRANSITION_TYPE type, float time_s, void (*cb)(void))
{
    must_init(cb, "transition callback");

    TRANSITION* pTransition = calloc(1, sizeof(TRANSITION));
    must_init(pTransition, "transition");
    pTransition->timer = asset_loader_load(A_TIMER, (AssetLoaderCallback)&create_timer);
    must_init(pTransition->timer, "transition timer");

    al_set_timer_speed(pTransition->timer, time_s / TRANSITION_STEPS);
    al_set_timer_count(pTransition->timer, 0);

    pTransition->callback = cb;
    pTransition->type = type;

    al_start_timer(pTransition->timer);

    return pTransition;
}

void transition_draw(void *_pTransition)
{
    TRANSITION* pTransition = (TRANSITION*)_pTransition;
    double step_percent = CLAMP((double)al_get_timer_count(pTransition->timer) / TRANSITION_STEPS, 0.0, 1.0);
    ALLEGRO_COLOR colour;

    switch (pTransition->type)
    {
    case NONE:
        break;
    case FADE_IN:
        colour = al_map_rgba_f(0, 0, 0, (1.0 - step_percent));
        al_draw_filled_rectangle(0, 0, BUFFER_W, BUFFER_H, colour);
        break;
    case FADE_OUT:
        colour = al_map_rgba_f(0, 0, 0, step_percent);
        al_draw_filled_rectangle(0, 0, BUFFER_W, BUFFER_H, colour);
        break;
    default:
        safe_exit("Invalid transition", 1);
        break;
    }
}

void transition_update(void *_pTransition, ALLEGRO_EVENT *pEvent)
{
    TRANSITION* pTransition = (TRANSITION*)_pTransition;
    
    if (al_get_timer_count(pTransition->timer) >= TRANSITION_STEPS)
    {
        al_stop_timer(pTransition->timer);
        al_set_timer_count(pTransition->timer, 0);
        pTransition->callback();
    }
}

static ALLEGRO_TIMER *create_timer(void)
{
    return al_create_timer(1.0);
}

void transition_free(void *_pTransition)
{
    TRANSITION* pTransition = (TRANSITION*)_pTransition;
    
    asset_loader_unload(pTransition->timer);
    free(pTransition);
}