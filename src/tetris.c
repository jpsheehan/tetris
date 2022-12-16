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
#include "main_menu.h"
#include "tetris.h"

typedef enum STATE
{
    MENU,
    GAME,
} STATE;

static ALLEGRO_TIMER *create_frame_timer(void);

static void menu_callback(CHOICE idx);
static void game_callback(void);

static bool done = false;
static STATE state = MENU;

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

    done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;
    int frames = 0;
    state = MENU;

    main_menu_init(menu_callback);

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

        switch (state)
        {
        case MENU:
            main_menu_update(&event);
            break;
        case GAME:
            game_update(&event, frames);
            break;
        default:
            safe_exit("Invalid state", 1);
            break;
        }

        if (redraw && al_is_event_queue_empty(queue))
        {
            disp_pre_draw();
            al_clear_to_color(al_map_rgb(0, 0, 0));

            switch (state)
            {
            case MENU:
                main_menu_draw();
                break;
            case GAME:
                game_draw();
                break;
            default:
                safe_exit("Invalid state", 1);
                break;
            }

            disp_post_draw();
            redraw = false;
        }
    }

    safe_exit(NULL, 0);

    return 0;
}

static ALLEGRO_TIMER *create_frame_timer(void)
{
    return al_create_timer(1.0 / (float)FPS);
}

static void game_callback(void)
{
    state = MENU;
}

static void menu_callback(CHOICE mode)
{
    switch (mode)
    {
    case CHOICE_EXIT:
        done = true;
        break;
    case CHOICE_MARATHON:
        state = GAME;
        game_init_marathon(&game_callback);
        break;
    case CHOICE_SPRINT:
        state = GAME;
        game_init_sprint(&game_callback);
        break;
    case CHOICE_ULTRA:
        state = GAME;
        game_init_ultra(&game_callback);
        break;
    case CHOICE_ENDLESS:
        state = GAME;
        game_init_endless(&game_callback);
        break;
#if DEBUG_MENU
    case CHOICE_DEBUG:
        state = GAME;
        game_init_debug(&game_callback);
        break;
#endif
    }
}