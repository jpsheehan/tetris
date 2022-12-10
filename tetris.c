#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include "utils.h"
#include "display.h"
#include "keyboard.h"
#include "field.h"

#define FPS 60

typedef enum PIECE
{
    O
} PIECE;

typedef struct PLAYER
{
    int x, y;
    ALLEGRO_COLOR c;
    PIECE piece;
} PLAYER;

PLAYER player;

bool player_collides_with_cell(PLAYER *p)
{
    switch (p->piece)
    {
    case O:
        return field_get_used_or_default(p->x, p->y, false) ||
               field_get_used_or_default(p->x + 1, p->y, false) ||
               field_get_used_or_default(p->x, p->y + 1, false) ||
               field_get_used_or_default(p->x + 1, p->y + 1, false);
    default:
        safe_exit("invalid piece in collides_with_cell()", 1);
        return false;
    }
}

void player_init()
{
    player.piece = O;
    player.x = FIELD_W / 2;
    player.y = -1;
    player.c = al_map_rgb_f(1, 1, 1);

    if (player_collides_with_cell(&player))
    {
        safe_exit("game over", 0);
    }
}

bool player_is_in_bounds(PLAYER *p)
{
    switch (p->piece)
    {
    case O:
        return p->x >= 0 && p->x + 1 < FIELD_W &&
               p->y >= 0 && p->y + 1 < FIELD_H;
    default:
        safe_exit("invalid piece in is_player_in_bounds()", 1);
        return false;
    }
}

bool player_can_move_down()
{
    PLAYER new_player = player;
    new_player.y++;

    return player_is_in_bounds(&new_player) && !player_collides_with_cell(&new_player);
}

bool player_can_move_right()
{
    PLAYER new_player = player;
    new_player.x++;

    return player_is_in_bounds(&new_player) && !player_collides_with_cell(&new_player);
}

bool player_can_move_left()
{
    PLAYER new_player = player;
    new_player.x--;

    return player_is_in_bounds(&new_player) && !player_collides_with_cell(&new_player);
}

bool player_can_rotate_cw()
{
    return false;
}

bool player_can_rotate_ccw()
{
    return false;
}

void player_lock_down()
{
    switch (player.piece)
    {
    case O:
        field_set_used_safely(player.x, player.y, player.c);
        field_set_used_safely(player.x + 1, player.y, player.c);
        field_set_used_safely(player.x, player.y + 1, player.c);
        field_set_used_safely(player.x + 1, player.y + 1, player.c);
        break;
    default:
        safe_exit("invalid piece in player_lock_down()", 1);
        break;
    }
}

void player_move_left()
{

    player.x--;
}

void player_move_right()
{

    player.x++;
}

void player_rotate_cw()
{
}

void player_rotate_ccw()
{
}

void player_move_down()
{
    player.y++;
}

void player_update(int frames)
{
    if (frames % FPS == 0) // each second
    {
        if (player_can_move_down())
        {
            player_move_down();
        }
        else
        {
            player_lock_down();
            player_init();
        }
    }

    if (frames % (FPS / 12) == 0) // 12 times per second
    {
        // MOVE LEFT
        if (keyboard_is_pressed(ALLEGRO_KEY_LEFT))
        {
            if (player_can_move_left())
            {
                player_move_left();
            }
        }

        // MOVE RIGHT
        if (keyboard_is_pressed(ALLEGRO_KEY_RIGHT))
        {
            if (player_can_move_right())
            {
                player_move_right();
            }
        }

        if (keyboard_is_pressed(ALLEGRO_KEY_Z))
        {
            if (player_can_rotate_cw())
            {
                player_rotate_cw();
            }
        }

        if (keyboard_is_pressed(ALLEGRO_KEY_X))
        {
            if (player_can_rotate_ccw())
            {
                player_can_rotate_ccw();
            }
        }

        // SOFT DROP
        if (keyboard_is_pressed(ALLEGRO_KEY_DOWN))
        {
            if (player_can_move_down())
            {
                player.y++;
            }
            else
            {
                player_lock_down();
                player_init();
            }
        }

        // HARD DROP
        if (keyboard_is_pressed(ALLEGRO_KEY_UP))
        {
            while (player_can_move_down())
            {
                player.y++;
            }
            player_lock_down();
            player_init();
        }
    }
}

void player_draw()
{
    switch (player.piece)
    {
    case O:
        field_draw_cell(player.x, player.y, player.c);
        field_draw_cell(player.x + 1, player.y, player.c);
        field_draw_cell(player.x, player.y + 1, player.c);
        field_draw_cell(player.x + 1, player.y + 1, player.c);
        break;
    default:
        safe_exit("invalid piece in player_draw()", 1);
        break;
    }
}

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

    player_init();
    field_init();

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
        player_update(frames);

        if (redraw && al_is_event_queue_empty(queue))
        {
            disp_pre_draw();
            al_clear_to_color(al_map_rgb(0, 0, 0));

            field_draw();
            player_draw();

            disp_post_draw();
            redraw = false;
        }
    }

    disp_deinit();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
