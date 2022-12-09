#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include "utils.h"
#include "display.h"
#include "keyboard.h"

extern ALLEGRO_DISPLAY* disp;

int main()
{
    must_init(al_init(), "allegro");

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    must_init(queue, "queue");

    keyboard_init();
    disp_init();

    must_init(al_init_primitives_addon(), "primitives");

    keyboard_register_event_source(queue);
    disp_register_event_source(queue);
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;
    int frames = 0;

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

        if (redraw && al_is_event_queue_empty(queue))
        {
            disp_pre_draw();
            al_clear_to_color(al_map_rgb(0, 0, 0));

            disp_post_draw();
            redraw = false;
        }
    }

    disp_deinit();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);


    return 0;
}

