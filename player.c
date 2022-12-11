#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "player.h"
#include "field.h"
#include "utils.h"
#include "keyboard.h"
#include "audio.h"
#include "randomiser.h"

static PLAYER player;
static PIECE held_piece;
static bool can_swap_with_held_piece;
static ALLEGRO_FONT *font;

bool player_collides_with_cell(PLAYER *p)
{
    ASSERT_PIECE(p->piece);
    ASSERT_ROTATION(p->rotation);

    for (int i = 0; i < 4; i++)
    {
        int x, y;
        mino_unmap_xy_offsets(p->piece, p->rotation, i, &x, &y);
        x += p->x;
        y += p->y;

        if (field_get_used_or_default(x, y, false))
            return true;
    }
    return false;
}

void dispense_specific_piece(PIECE piece)
{
    player.piece = piece;
    player.x = (FIELD_W - 4) / 2;
    player.y = -1;
    player.rotation = 0;
    player.c = mino_get_default_colour(player.piece);

    if (player_collides_with_cell(&player))
        safe_exit("game over", 0);
}

void dispense_next_piece()
{
    dispense_specific_piece(randomiser_next());
    can_swap_with_held_piece = true;
}

void player_init()
{
    font = al_create_builtin_font();
    held_piece = PIECE_MAX;
    dispense_next_piece();
}

void player_deinit()
{
    if (font != NULL)
    {
        al_destroy_font(font);
        font = NULL;
    }
}

bool player_is_in_bounds(PLAYER *p)
{
    ASSERT_PIECE(p->piece);
    ASSERT_ROTATION(p->rotation);

    for (int i = 0; i < 4; i++)
    {
        int x, y;
        mino_unmap_xy_offsets(p->piece, p->rotation, i, &x, &y);
        x += p->x;
        y += p->y;

        if (x < 0 || x >= FIELD_W || y < -1 || y >= FIELD_H)
            return false;
    }
    return true;
}

void player_move_left(PLAYER *p)
{
    p->x--;
}

void player_move_right(PLAYER *p)
{
    p->x++;
}

void player_move_down(PLAYER *p)
{
    p->y++;
}

void player_rotate_cw(PLAYER *p)
{
    p->rotation = (p->rotation + 1) % 4;

    if (p == &player)
        audio_play_sfx(SFX_ROTATE_CW);
}

void player_rotate_ccw(PLAYER *p)
{
    p->rotation = (p->rotation + 3) % 4;

    if (p == &player)
        audio_play_sfx(SFX_ROTATE_CCW);
}

bool player_can_move_down(PLAYER *p)
{
    PLAYER new_player = *p;
    player_move_down(&new_player);

    return player_is_in_bounds(&new_player) && !player_collides_with_cell(&new_player);
}

bool player_can_move_right(PLAYER *p)
{
    PLAYER new_player = *p;
    player_move_right(&new_player);

    return player_is_in_bounds(&new_player) && !player_collides_with_cell(&new_player);
}

bool player_can_move_left(PLAYER *p)
{
    PLAYER new_player = *p;
    player_move_left(&new_player);

    return player_is_in_bounds(&new_player) && !player_collides_with_cell(&new_player);
}

bool player_can_rotate_cw(PLAYER *p)
{
    PLAYER new_player = *p;
    player_rotate_cw(&new_player);

    return player_is_in_bounds(&new_player) && !player_collides_with_cell(&new_player);
}

bool player_can_rotate_ccw(PLAYER *p)
{
    PLAYER new_player = *p;
    player_rotate_ccw(&new_player);

    return player_is_in_bounds(&new_player) && !player_collides_with_cell(&new_player);
}

void player_lock_down()
{
    ASSERT_PIECE(player.piece);
    ASSERT_ROTATION(player.rotation);

    for (int i = 0; i < 4; i++)
    {
        int x, y;
        mino_unmap_xy_offsets(player.piece, player.rotation, i, &x, &y);
        x += player.x;
        y += player.y;

        field_set_used_safely(x, y, player.c);
    }
}

void player_update(ALLEGRO_EVENT *event, int frames)
{
    if (frames % FPS == 0) // each second
    {
        if (player_can_move_down(&player))
        {
            player_move_down(&player);
        }
        else
        {
            player_lock_down();
            dispense_next_piece();
            audio_play_sfx(SFX_LOCK_DOWN);
        }
    }

    if (frames % (FPS / 20) == 0) // 20 times per second
    {
        // MOVE LEFT
        if (keyboard_is_pressed(ALLEGRO_KEY_LEFT))
        {
            if (player_can_move_left(&player))
            {
                player_move_left(&player);
            }
        }

        // MOVE RIGHT
        if (keyboard_is_pressed(ALLEGRO_KEY_RIGHT))
        {
            if (player_can_move_right(&player))
            {
                player_move_right(&player);
            }
        }

        // SOFT DROP
        if (keyboard_is_pressed(ALLEGRO_KEY_DOWN))
        {
            if (player_can_move_down(&player))
            {
                player_move_down(&player);
            }
            else
            {
                player_lock_down();
                dispense_next_piece();
                audio_play_sfx(SFX_LOCK_DOWN);
            }
        }
    }

    if (event->type == ALLEGRO_EVENT_KEY_DOWN)
    {
        switch (event->keyboard.keycode)
        {
        case ALLEGRO_KEY_X:
            if (player_can_rotate_cw(&player))
            {
                player_rotate_cw(&player);
            };
            break;
        case ALLEGRO_KEY_Z:
            if (player_can_rotate_ccw(&player))
            {
                player_rotate_ccw(&player);
            }
            break;
        case ALLEGRO_KEY_UP: // HARD DROP
            while (player_can_move_down(&player))
            {
                player_move_down(&player);
            }
            player_lock_down();
            dispense_next_piece();
            audio_play_sfx(SFX_HARD_DROP);
            break;
        case ALLEGRO_KEY_SPACE: // SWAP
            if (can_swap_with_held_piece)
            {
                if (held_piece >= 0 && held_piece < PIECE_MAX)
                {
                    PIECE temp = held_piece;
                    held_piece = player.piece;
                    dispense_specific_piece(temp);
                }
                else
                {
                    held_piece = player.piece;
                    dispense_next_piece();
                }
                can_swap_with_held_piece = false;
            }
            break;
        }
    }
}

void draw_ghost_piece()
{
    if (!player_can_move_down(&player))
        return;

    PLAYER ghost = player;
    while (player_can_move_down(&ghost))
    {
        player_move_down(&ghost);
    }

    field_draw_mino(ghost.piece, ghost.rotation, ghost.x, ghost.y, al_map_rgba_f(0.5, 0.5, 0.5, 0.3));
}

void player_draw()
{
    ASSERT_PIECE(player.piece);
    ASSERT_ROTATION(player.rotation);

    draw_ghost_piece();

    // draw player
    field_draw_mino(player.piece, player.rotation, player.x, player.y, player.c);

    // draw held piece
    int held_piece_ui_offset_x = 20;
    int held_piece_ui_offset_y = 40;
    al_draw_textf(font, al_map_rgb_f(1,1,1), held_piece_ui_offset_x, held_piece_ui_offset_y, 0, "Hold");
    
    if (held_piece >= 0 && held_piece < PIECE_MAX)
    {
        mino_draw(held_piece, 0, held_piece_ui_offset_x, held_piece_ui_offset_y + 10, mino_get_default_colour(held_piece), 0.7);
    }
}