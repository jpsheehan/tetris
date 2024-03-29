#include <stdio.h>
#include <allegro5/allegro.h>

#include "player.h"
#include "field.h"
#include "utils.h"
#include "keyboard.h"
#include "audio.h"
#include "randomiser.h"
#include "score.h"
#include "asset_loader.h"
#include "input.h"

#define CLEAR_TIMER_PERIOD 0.4

static void (*callback)(void);
static TSPIN get_tspin(void);
static bool player_rotate_cw(PLAYER *p);
static bool player_rotate_ccw(PLAYER *p);

typedef enum ROTATION
{
    NONE,
    CW,
    CCW
} ROTATION;

static PLAYER player;
static PIECE held_piece;
static bool can_swap_with_held_piece;
static int lock_delay = 0;
static int gravity_timer = 0;
static int clear_timer = 0;
static TSPIN tspin_state = TS_NONE;
static bool should_dispense_next_piece = false;

static bool was_last_move_a_rotation = false;
static int last_kick_index = -1;
static ROTATION prespawn_rotation = NONE;

static bool player_collides_with_cell(PLAYER *p)
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

static void dispense_specific_piece(PIECE piece)
{
    player.piece = piece;
    player.x = (FIELD_W - 4) / 2;
    player.y = -1;
    player.rotation = 0;
    player.c = mino_get_default_colour(player.piece);

    if (player_collides_with_cell(&player))
    {
        callback();
    }
}

static void dispense_next_piece()
{
    al_stop_timer(A(lock_delay));
    al_stop_timer(A(gravity_timer));
    al_stop_timer(A(clear_timer));
    al_set_timer_count(A(clear_timer), 0);

    dispense_specific_piece(randomiser_next());
    can_swap_with_held_piece = true;
    should_dispense_next_piece = false;

    switch (prespawn_rotation)
    {
        case NONE: break;
        case CW: player_rotate_cw(&player); break;
        case CCW: player_rotate_ccw(&player); break;
        default: safe_exit("Invalid rotation", 1); break;
    }

    prespawn_rotation = NONE;
}

static ALLEGRO_TIMER *create_lock_delay_timer(void)
{
    return al_create_timer(0.5);
}

static ALLEGRO_TIMER *create_gravity_timer(void)
{
    return al_create_timer(gravity_get(level_get()));
}

static ALLEGRO_TIMER *create_clear_timer(void)
{
    return al_create_timer(CLEAR_TIMER_PERIOD / CLEAR_TIMER_RESOLUTION);
}

TSPIN player_get_tspin_state(void)
{
    return tspin_state;
}

void player_pause_timers(void)
{
    al_stop_timer(A(gravity_timer));
    al_stop_timer(A(clear_timer));
}

void player_start_timers(void)
{
    al_start_timer(A(gravity_timer));
    al_start_timer(A(clear_timer));
}

void player_init(void (*cb)(void))
{
    callback = cb;

    if (lock_delay == 0)
    {
        lock_delay = asset_loader_load("lock delay timer", A_TIMER, (AssetLoaderCallback)&create_lock_delay_timer);
    }

    if (gravity_timer == 0)
    {
        gravity_timer = asset_loader_load("gravity timer", A_TIMER, (AssetLoaderCallback)&create_gravity_timer);
    }

    if (clear_timer == 0)
    {
        clear_timer = asset_loader_load("clear timer", A_TIMER, (AssetLoaderCallback)&create_clear_timer);
    }

    held_piece = PIECE_MAX;
    was_last_move_a_rotation = false;
    last_kick_index = -1;
    dispense_next_piece();
}

void reset_lock_delay(void)
{
    if (lock_delay != 0)
    {
        if (al_get_timer_started(A(lock_delay)))
            al_stop_timer(A(lock_delay));
        al_set_timer_count(A(lock_delay), 0);
        al_start_timer(A(lock_delay));
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

bool player_rotate(PLAYER *p, bool ccw)
{
    int initial_rotation = p->rotation;
    int initial_x = p->x;
    int initial_y = p->y;
    last_kick_index = -1;

    p->rotation = (p->rotation + (ccw ? 3 : 1)) % 4;

    if (player_is_in_bounds(p) && !player_collides_with_cell(p))
    {
        return true;
    }

    POINT *kicks = mino_get_kick_data(p->piece, initial_rotation, false);
    if (kicks == NULL)
    {
        p->rotation = initial_rotation;
        return false;
    }

    for (int i = 0; i < 4; i++)
    {
        POINT kick = kicks[i];
        p->x = initial_x + kick.x;
        p->y = initial_y + kick.y;

        if (player_is_in_bounds(p) && !player_collides_with_cell(p))
        {
            last_kick_index = i;
            return true;
        }
    }

    p->rotation = initial_rotation;
    p->x = initial_x;
    p->y = initial_y;
    return false;
}

bool player_rotate_ccw(PLAYER *p)
{
    return player_rotate(p, true);
}

bool player_rotate_cw(PLAYER *p)
{
    return player_rotate(p, false);
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

bool player_lock_down(bool hard_lock)
{
#if !MAKE_LOGO
    ASSERT_PIECE(player.piece);
    ASSERT_ROTATION(player.rotation);

    if (!hard_lock)
    {
        if (!al_get_timer_started(A(lock_delay)))
        {
            reset_lock_delay();
        }

        if (!al_get_timer_count(A(lock_delay)))
        {
            return false;
        }
    }

    al_stop_timer(A(lock_delay));
#endif

    int max_y = -10;
    for (int i = 0; i < 4; i++)
    {
        int x, y;
        mino_unmap_xy_offsets(player.piece, player.rotation, i, &x, &y);
        x += player.x;
        y += player.y;

        max_y = MAX(y, max_y);

        field_set_used_safely(x, y, player.c);
    }

    if (max_y < 0)
    {
        callback();
    }

    tspin_state = get_tspin();

    return true;
}

long player_get_clear_timer_count(void)
{
    if (!al_get_timer_started(A(clear_timer)))
        return -1;

    return al_get_timer_count(A(clear_timer));
}

void player_update(ALLEGRO_EVENT *event, int frames)
{
    static int previous_level = 1;
    static int previous_lines_cleared = 0;

    tspin_state = TS_NONE;

    if (event->type == ALLEGRO_EVENT_TIMER)
    {
        if (should_dispense_next_piece)
        {
            if (previous_lines_cleared != lines_cleared_get())
            {
                if (al_get_timer_started(A(clear_timer)))
                {
                    if (al_get_timer_count(A(clear_timer)) >= CLEAR_TIMER_RESOLUTION)
                    {
                        field_clear_rows();
                        dispense_next_piece();
                        previous_lines_cleared = lines_cleared_get();
                    }
                }
                else
                {
                    al_start_timer(A(clear_timer));
                }
            }
            else
            {
                // no lines were cleared so just dispense a piece
                dispense_next_piece();
            }
        }

        if (should_dispense_next_piece)
            return;

        if (!al_get_timer_started(A(gravity_timer)))
        {
            al_set_timer_count(A(gravity_timer), 0);
            al_set_timer_speed(A(gravity_timer), gravity_get(level_get()));
            al_start_timer(A(gravity_timer));
        }

        if (previous_level != level_get())
        {
            al_set_timer_speed(A(gravity_timer), gravity_get(level_get()));
            previous_level = level_get();
        }

        int64_t gravity = al_get_timer_count(A(gravity_timer));
        for (int _ = 0; _ < gravity; _++)
        {
            if (player_can_move_down(&player))
            {
                player_move_down(&player);
            }
            else
            {
                if (player_lock_down(false))
                {
                    should_dispense_next_piece = true;
                    set_requires_update();
                    audio_play_sfx(SFX_LOCK_DOWN);
                }
                break;
            }
        }
        al_add_timer_count(A(gravity_timer), -(gravity));
    }

    if (should_dispense_next_piece)
    {
        // pre-spawn rotations
        if (event->type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (prespawn_rotation == NONE)
            {
                if (event->keyboard.keycode == input_get_mapping(INPUT_ROTATE_CW))
                {
                    prespawn_rotation = CW;
                    audio_play_sfx(SFX_PRESPAWN_ROTATE);
                }
                else if (event->keyboard.keycode == input_get_mapping(INPUT_ROTATE_CCW))
                {
                    prespawn_rotation = CCW;
                    audio_play_sfx(SFX_PRESPAWN_ROTATE);
                }
            }
        }
    }
    else
    {
        if (frames % (FPS / 10) == 0)
        {
            if (keyboard_is_pressed(input_get_mapping(INPUT_MOVE_LEFT)))
            {
                if (player_can_move_left(&player))
                {
                    was_last_move_a_rotation = false;
                    player_move_left(&player);
                    if (al_get_timer_started(A(lock_delay)))
                    {
                        reset_lock_delay();
                    }
                }
            }

            // MOVE RIGHT
            if (keyboard_is_pressed(input_get_mapping(INPUT_MOVE_RIGHT)))
            {
                if (player_can_move_right(&player))
                {
                    was_last_move_a_rotation = false;
                    player_move_right(&player);
                    if (al_get_timer_started(A(lock_delay)))
                    {
                        reset_lock_delay();
                    }
                }
            }

            // SOFT DROP
            if (keyboard_is_pressed(input_get_mapping(INPUT_SOFT_DROP)))
            {
                was_last_move_a_rotation = false;
                int blocks_to_drop = 2;

                for (int i = 0; i < blocks_to_drop; i++)
                {
                    if (player_can_move_down(&player))
                    {
                        player_move_down(&player);
                        score_add(SOFT_DROP);
                    }
                    else
                    {
                        if (player_lock_down(true))
                        {
                            keyboard_reset_key(input_get_mapping(INPUT_SOFT_DROP));
                            should_dispense_next_piece = true;
                            set_requires_update();
                            audio_play_sfx(SFX_LOCK_DOWN);
                        }
                        break;
                    }
                }
            }
        }

        if (event->type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (event->keyboard.keycode == input_get_mapping(INPUT_ROTATE_CW))
            {
                if (player_rotate_cw(&player))
                {
                    was_last_move_a_rotation = true;
                    if (al_get_timer_started(A(lock_delay)))
                    {
                        reset_lock_delay();
                    }
                    audio_play_sfx(SFX_ROTATE_CW);
                }
            }
            else if (event->keyboard.keycode == input_get_mapping(INPUT_ROTATE_CCW))
            {
                if (player_rotate_ccw(&player))
                {
                    was_last_move_a_rotation = true;
                    if (al_get_timer_started(A(lock_delay)))
                    {
                        reset_lock_delay();
                    }
                    audio_play_sfx(SFX_ROTATE_CCW);
                }
            }
            else if (event->keyboard.keycode == input_get_mapping(INPUT_HARD_DROP))
            {
                was_last_move_a_rotation = false;
                while (player_can_move_down(&player))
                {
                    player_move_down(&player);
                    score_add(HARD_DROP);
                }
                if (player_lock_down(true))
                {
                    should_dispense_next_piece = true;
                    set_requires_update();
                    audio_play_sfx(SFX_HARD_DROP);
                }
            }
            else if (event->keyboard.keycode == input_get_mapping(INPUT_HOLD))
            {
                if (can_swap_with_held_piece)
                {
                    was_last_move_a_rotation = false;
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
                    al_stop_timer(A(lock_delay));
                }
            }
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

    field_draw_mino(ghost.piece, ghost.rotation, ghost.x, ghost.y, al_map_rgb_f(0.5, 0.5, 0.5));
}

void player_draw()
{
    ASSERT_PIECE(player.piece);
    ASSERT_ROTATION(player.rotation);

    if (should_dispense_next_piece)
        return;

    if (level_get() < 15)
        draw_ghost_piece();

    // draw player
    field_draw_mino(player.piece, player.rotation, player.x, player.y, player.c);
}

PIECE player_get_held_piece(void)
{
    return held_piece;
}

static TSPIN get_tspin(void)
{
    // https://tetris.wiki/T-Spin#Current_rules
    if (player.piece == T && was_last_move_a_rotation)
    {
        int corners_filled = 0;
        int front_corners_filled = 0;

        if (field_get_used_or_default(player.x, player.y, true))
            corners_filled++;
        if (field_get_used_or_default(player.x + 2, player.y, true))
            corners_filled++;
        if (field_get_used_or_default(player.x, player.y + 2, true))
            corners_filled++;
        if (field_get_used_or_default(player.x + 2, player.y + 2, true))
            corners_filled++;

        switch (player.rotation)
        {
        case 0:
            if (field_get_used_or_default(player.x, player.y, true))
                front_corners_filled++;
            if (field_get_used_or_default(player.x + 2, player.y, true))
                front_corners_filled++;
            break;
        case 1:
            if (field_get_used_or_default(player.x + 2, player.y, true))
                front_corners_filled++;
            if (field_get_used_or_default(player.x + 2, player.y + 2, true))
                front_corners_filled++;
            break;
        case 2:
            if (field_get_used_or_default(player.x, player.y + 2, true))
                front_corners_filled++;
            if (field_get_used_or_default(player.x + 2, player.y + 2, true))
                front_corners_filled++;
            break;
        case 3:
            if (field_get_used_or_default(player.x, player.y, true))
                front_corners_filled++;
            if (field_get_used_or_default(player.x, player.y + 2, true))
                front_corners_filled++;
            break;
        }

        if (corners_filled >= 3)
        {
            if (front_corners_filled == 2)
            {
                return TS_PROPER;
            }
            else
            {
                if (last_kick_index == 3)
                {
                    return TS_PROPER;
                }
                else
                {
                    return TS_MINI;
                }
            }
        }
    }

    return TS_NONE;
}

#if MAKE_LOGO
void player_make_logo(void)
{
    while (player_can_move_down(&player))
    {
        player_move_down(&player);
    }
    player_lock_down(true);

    dispense_next_piece();
    player_rotate_cw(&player);
    player_move_left(&player);
    player_move_left(&player);
    player_move_left(&player);
    while (player_can_move_down(&player))
    {
        player_move_down(&player);
    }
    player_lock_down(true);

    dispense_next_piece();
    player.c = mino_get_default_colour(O);
    player_rotate_cw(&player);
    player_move_left(&player);
    while (player_can_move_down(&player))
    {
        player_move_down(&player);
    }
    player_lock_down(true);

    dispense_next_piece();
    player.c = mino_get_default_colour(Z);
    player_rotate_ccw(&player);
    player_move_right(&player);
    while (player_can_move_down(&player))
    {
        player_move_down(&player);
    }
    player_lock_down(true);
    dispense_next_piece();
}
#endif