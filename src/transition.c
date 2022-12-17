#include <allegro5/allegro_primitives.h>
#include <stdio.h>

#include "transition.h"
#include "asset_loader.h"
#include "utils.h"
#include "display.h"

#define TRANSITION_STEPS 60.0

static ALLEGRO_TIMER *create_timer(void);

static void (*callback)(void) = NULL;
static ALLEGRO_TIMER *timer = NULL;
static TRANSITION current_transition = NONE;

void transition_init(void)
{
    if (timer == NULL)
    {
        timer = asset_loader_load(A_TIMER, (AssetLoaderCallback)&create_timer);
        must_init(timer, "transition timer");
    }
}

void transition_start(TRANSITION transition, float time_s, void (*cb)(void))
{
    must_init(cb, "transition callback");

    al_set_timer_speed(timer, time_s / TRANSITION_STEPS);
    al_set_timer_count(timer, 0);

    callback = cb;
    current_transition = transition;

    al_start_timer(timer);
}

void transition_draw(void)
{
    double step_percent = CLAMP((double)al_get_timer_count(timer) / TRANSITION_STEPS, 0.0, 1.0);
    ALLEGRO_COLOR colour;

    switch (current_transition)
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

void transition_update(ALLEGRO_EVENT *pEvent)
{
    if (al_get_timer_count(timer) >= TRANSITION_STEPS)
    {
        al_stop_timer(timer);

        callback();
    }
}

static ALLEGRO_TIMER *create_timer(void)
{
    return al_create_timer(1.0);
}