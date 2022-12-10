#include "player.h"
#include "field.h"
#include "utils.h"
#include "keyboard.h"
#include "audio.h"

static PLAYER player;

typedef struct POINT
{
    int x, y;
} POINT;

typedef struct ROTATION
{
    POINT ps[4];
} ROTATION;

static POINT rotations[PIECE_MAX][4][4] = {
    {// O
     {
         // ##
         // ##
         {.x = 0, .y = 0},
         {.x = 1, .y = 0},
         {.x = 0, .y = 1},
         {.x = 1, .y = 1},
     },
     {
         // ##
         // ##
         {.x = 0, .y = 0},
         {.x = 1, .y = 0},
         {.x = 0, .y = 1},
         {.x = 1, .y = 1},
     },
     {
         // ##
         // ##
         {.x = 0, .y = 0},
         {.x = 1, .y = 0},
         {.x = 0, .y = 1},
         {.x = 1, .y = 1},
     },
     {
         // ##
         // ##
         {.x = 0, .y = 0},
         {.x = 1, .y = 0},
         {.x = 0, .y = 1},
         {.x = 1, .y = 1},
     }}};

bool player_collides_with_cell(PLAYER *p)
{
    if (p->piece < 0 || p->piece >= PIECE_MAX)
        safe_exit("invalid piece in player_collides_with_cell()", 1);
    if (p->rotation < 0 || p->rotation >= 4)
        safe_exit("invalid rotation in player_collides_with_cell()", 1);

    for (int i = 0; i < 4; i++)
    {
        int x = p->x + rotations[p->piece][p->rotation][i].x;
        int y = p->y + rotations[p->piece][p->rotation][i].y;

        if (field_get_used_or_default(x, y, false))
            return true;
    }
    return false;
}

ALLEGRO_COLOR player_get_default_colour(PIECE piece)
{
    switch (piece)
    {
    case O:
        return al_map_rgb(0xEF, 0xE7, 0x10);
    // case T:
    //     return al_map_rgb(0xc2, 0x2d, 0xd2);
    default:
        safe_exit("invalid piece in player_get_default_colour()", 1);
        return al_map_rgb(0, 0, 0);
    }
}

void player_init()
{
    player.piece = (PIECE)(rand() % PIECE_MAX);
    player.x = FIELD_W / 2 - 1;
    player.y = -1;
    player.rotation = 0;

    player.c = player_get_default_colour(player.piece);

    if (player_collides_with_cell(&player))
        safe_exit("game over", 0);
}

bool player_is_in_bounds(PLAYER *p)
{
    if (p->piece < 0 || p->piece >= PIECE_MAX)
        safe_exit("piece is invalid in player_is_in_bounds()", 1);
    if (p->rotation < 0 || p->rotation >= 4)
        safe_exit("rotation is invalid in player_is_in_bounds()", 1);

    for (int i = 0; i < 4; i++)
    {
        int x = p->x + rotations[p->piece][p->rotation][i].x;
        int y = p->y + rotations[p->piece][p->rotation][i].y;

        if (x < 0 || x >= FIELD_W || y < 0 || y >= FIELD_H)
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
    switch (p->piece)
    {
    case O:
        p->rotation = (p->rotation + 1) % 4;
        break;
    default:
        safe_exit("invalid piece in player_rotate_cw()", 1);
        break;
    }

    if (p == &player)
        audio_play_sfx(SFX_ROTATE_CW);
}

void player_rotate_ccw(PLAYER *p)
{
    switch (p->piece)
    {
    case O:
        p->rotation = (p->rotation + 3) % 4;
        break;
    default:
        safe_exit("invalid piece in player_rotate_ccw()", 1);
        break;
    }

    if (p == &player)
        audio_play_sfx(SFX_ROTATE_CCW);
}

bool player_can_move_down()
{
    PLAYER new_player = player;
    player_move_down(&new_player);

    return player_is_in_bounds(&new_player) && !player_collides_with_cell(&new_player);
}

bool player_can_move_right()
{
    PLAYER new_player = player;
    player_move_right(&new_player);

    return player_is_in_bounds(&new_player) && !player_collides_with_cell(&new_player);
}

bool player_can_move_left()
{
    PLAYER new_player = player;
    player_move_left(&new_player);

    return player_is_in_bounds(&new_player) && !player_collides_with_cell(&new_player);
}

bool player_can_rotate_cw()
{
    PLAYER new_player = player;
    player_rotate_cw(&new_player);

    return player_is_in_bounds(&new_player) && !player_collides_with_cell(&new_player);
}

bool player_can_rotate_ccw()
{
    PLAYER new_player = player;
    player_rotate_ccw(&new_player);

    return player_is_in_bounds(&new_player) && !player_collides_with_cell(&new_player);
}

void player_lock_down()
{
    if (player.piece < 0 || player.piece >= PIECE_MAX)
        safe_exit("piece is invalid in player_lock_down()", 1);
    if (player.rotation < 0 || player.rotation >= 4)
        safe_exit("rotation is invalid in player_lock_down()", 1);

    for (int i = 0; i < 4; i++)
    {
        int x = player.x + rotations[player.piece][player.rotation][i].x;
        int y = player.y + rotations[player.piece][player.rotation][i].y;

        field_set_used_safely(x, y, player.c);
    }
}

void player_update(ALLEGRO_EVENT *event, int frames)
{
    if (frames % FPS == 0) // each second
    {
        if (player_can_move_down())
        {
            player_move_down(&player);
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
                player_move_left(&player);
            }
        }

        // MOVE RIGHT
        if (keyboard_is_pressed(ALLEGRO_KEY_RIGHT))
        {
            if (player_can_move_right())
            {
                player_move_right(&player);
            }
        }

        // SOFT DROP
        if (keyboard_is_pressed(ALLEGRO_KEY_DOWN))
        {
            if (player_can_move_down())
            {
                player_move_down(&player);
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
                player_rotate_cw(&player);
            }
        }

        if (keyboard_is_just_pressed(ALLEGRO_KEY_X))
        {
            if (player_can_rotate_ccw())
            {
                player_rotate_ccw(&player);
            }
        }

        // HARD DROP
        if (keyboard_is_just_pressed(ALLEGRO_KEY_UP))
        {
            while (player_can_move_down())
            {
                player_move_down(&player);
            }
            player_lock_down();
            player_init();
            audio_play_sfx(SFX_HARD_DROP);
        }
    }
}

void player_draw()
{
    if (player.piece < 0 || player.piece >= PIECE_MAX || player.rotation < 0 || player.rotation >= 4)
        safe_exit("invalid piece or rotation in player_draw()", 1);

    for (int i = 0; i < 4; i++)
    {
        int x = player.x + rotations[player.piece][player.rotation][i].x;
        int y = player.y + rotations[player.piece][player.rotation][i].y;
        field_draw_cell(x, y, player.c);
    }
}