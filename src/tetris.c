#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "utils.h"
#include "display.h"
#include "keyboard.h"
#include "field.h"
#include "player.h"
#include "score.h"
#include "audio.h"
#include "randomiser.h"
#include "hud.h"

int main()
{
    must_init(al_init(), "allegro");

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / (float)FPS);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    must_init(queue, "queue");

    disp_init();
    keyboard_init();
    audio_init();

    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_init_font_addon(), "font");

    keyboard_register_event_source(queue);
    disp_register_event_source(queue);
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;
    int frames = 0;

    randomiser_init();
    player_init();
    field_init();
    score_init();
    hud_init();

    al_start_timer(timer);

    while (1)
    {
        al_wait_for_event(queue, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_TIMER:

            if (keyboard_is_pressed(ALLEGRO_KEY_ESCAPE))
                done = true;

            redraw = true;
            frames++;
            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }

        if (done)
            break;

        keyboard_update(&event);
        player_update(&event, frames);
        field_update();
        hud_update();

        if (redraw && al_is_event_queue_empty(queue))
        {
            disp_pre_draw();
            al_clear_to_color(al_map_rgb(0, 0, 0));

            field_draw();
            player_draw();
            hud_draw();

            disp_post_draw();
            redraw = false;
        }
    }

    player_deinit();
    hud_deinit();
    audio_deinit();
    disp_deinit();
    randomiser_deinit();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
