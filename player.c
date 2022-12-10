#include "player.h"
#include "field.h"
#include "utils.h"
#include "keyboard.h"
#include "audio.h"

static PLAYER player;

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
        safe_exit("invalid piece in player_collides_with_cell()", 1);
        return false;
    }
}

ALLEGRO_COLOR player_get_default_colour(PIECE piece)
{
    switch (piece)
    {
    case O:
        return al_map_rgb(0xEF, 0xE7, 0x10);
    default:
        safe_exit("invalid piece in player_get_default_colour()", 1);
        return al_map_rgb(0, 0, 0);
    }
}

void player_init()
{
    player.piece = (PIECE)(rand() % PIECE_MAX);
    player.x = FIELD_W / 2;
    player.y = -1;

    player.c = player_get_default_colour(player.piece);

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
        safe_exit("invalid piece in player_is_in_bounds()", 1);
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
    switch (player.piece)
    {
    case O:
        return true;
    default:
        safe_exit("invalid piece in player_can_rotate_cw()", 1);
        return false;
    }
}

bool player_can_rotate_ccw()
{
    switch (player.piece)
    {
    case O:
        return true;
    default:
        safe_exit("invalid piece in player_can_rotate_ccw()", 1);
        return false;
    }
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
    switch (player.piece)
    {
    case O:
        break;
    default:
        safe_exit("invalid piece in player_rotate_cw()", 1);
        break;
    }
    audio_play_sfx(SFX_ROTATE_CW);
}

void player_rotate_ccw()
{
    switch (player.piece)
    {
    case O:
        break;
    default:
        safe_exit("invalid piece in player_rotate_ccw()", 1);
        break;
    }
    audio_play_sfx(SFX_ROTATE_CCW);
}

void player_move_down()
{
    player.y++;
}

void player_update(ALLEGRO_EVENT *event, int frames)
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
            audio_play_sfx(SFX_LOCK_DOWN);
        }
    }

    if (frames % (FPS / 20) == 0) // 20 times per second
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
                audio_play_sfx(SFX_LOCK_DOWN);
            }
        }
    }

    if (event->type == ALLEGRO_EVENT_KEY_DOWN)
    {
        if (keyboard_is_just_pressed(ALLEGRO_KEY_Z))
        {
            if (player_can_rotate_cw())
            {
                player_rotate_cw();
            }
        }

        if (keyboard_is_just_pressed(ALLEGRO_KEY_X))
        {
            if (player_can_rotate_ccw())
            {
                player_rotate_ccw();
            }
        }

        // HARD DROP
        if (keyboard_is_just_pressed(ALLEGRO_KEY_UP))
        {
            while (player_can_move_down())
            {
                player.y++;
            }
            player_lock_down();
            player_init();
            audio_play_sfx(SFX_HARD_DROP);
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