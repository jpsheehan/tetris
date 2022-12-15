#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "utils.h"
#include "display.h"
#include "keyboard.h"
#include "audio.h"
#include "game.h"
#include "asset_loader.h"

ALLEGRO_TIMER *create_frame_timer(void)
{
    return al_create_timer(1.0 / (float)FPS);
}

int main()
{
    must_init(al_init(), "allegro");
    must_init(al_install_audio(), "install audio");
    must_init(al_init_acodec_addon(), "init audio codec");
    must_init(al_reserve_samples(16), "reserve samples");

    asset_loader_init();

    disp_init();
    keyboard_init();
    must_init(al_install_keyboard(), "keyboard");

    audio_init();

    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_init_font_addon(), "font");

    ALLEGRO_TIMER *timer = asset_loader_load(A_TIMER, (AssetLoaderCallback)&create_frame_timer);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE *queue = asset_loader_load(A_EVENT_QUEUE, (AssetLoaderCallback)&al_create_event_queue);
    must_init(queue, "queue");

    keyboard_register_event_source(queue);
    disp_register_event_source(queue);
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;
    int frames = 0;

    game_init();

    al_start_timer(timer);

    while (1)
    {
        al_wait_for_event(queue, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_TIMER:

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

        game_update(&event, frames);

        if (redraw && al_is_event_queue_empty(queue))
        {
            disp_pre_draw();
            al_clear_to_color(al_map_rgb(0, 0, 0));

            game_draw();

            disp_post_draw();
            redraw = false;
        }
    }

    safe_exit("Done", 0);

    return 0;
}
