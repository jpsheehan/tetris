#include "player.h"
#include "field.h"
#include "utils.h"
#include "keyboard.h"
#include "audio.h"
#include <stdio.h>

#define ASSERT_RANGE(x, min, max, name)                                         \
    do                                                                          \
    {                                                                           \
        if ((x) < (min) || (x) >= (max))                                        \
        {                                                                       \
            char __message[100] = {0};                                          \
            snprintf(__message, 100, "invalid %s in %s", (name), __FUNCTION__); \
            safe_exit(__message, 1);                                            \
        }                                                                       \
    } while (0)

#define ASSERT_PIECE(piece) ASSERT_RANGE(piece, 0, PIECE_MAX, "piece")
#define ASSERT_ROTATION(rot) ASSERT_RANGE(rot, 0, 4, "rotation")

static PLAYER player;

typedef struct POINT
{
    int x, y;
} POINT;

typedef struct ROTATION
{
    POINT ps[4];
} ROTATION;

static unsigned char colours[PIECE_MAX][3] = {
    {0xEF, 0xE7, 0x10}, // O
    {0xc2, 0x2d, 0xd2}, // T
};

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
     }},
    {// T
     {
         //  #
         // ###
         {.x = 1, .y = 0},
         {.x = 0, .y = 1},
         {.x = 1, .y = 1},
         {.x = 2, .y = 1},
     },
     {
         // #
         // ##
         // #
         {.x = 0, .y = 0},
         {.x = 0, .y = 1},
         {.x = 0, .y = 2},
         {.x = 1, .y = 1},
     },
     {
         // ###
         //  #
         {.x = 1, .y = 1},
         {.x = 0, .y = 0},
         {.x = 1, .y = 0},
         {.x = 2, .y = 0},
     },
     {
         //  #
         // ##
         //  #
         {.x = 1, .y = 0},
         {.x = 1, .y = 1},
         {.x = 1, .y = 2},
         {.x = 0, .y = 1},
     }}};

bool player_collides_with_cell(PLAYER *p)
{
    ASSERT_PIECE(p->piece);
    ASSERT_ROTATION(p->rotation);

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
    ASSERT_PIECE(piece);

    unsigned char r = colours[piece][0];
    unsigned char g = colours[piece][1];
    unsigned char b = colours[piece][2];

    return al_map_rgb(r, g, b);
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
    ASSERT_PIECE(p->piece);
    ASSERT_ROTATION(p->rotation);

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
    p->rotation = (p->rotation + 3) % 4;

    if (p == &player)
        audio_play_sfx(SFX_ROTATE_CW);
}

void player_rotate_ccw(PLAYER *p)
{
    p->rotation = (p->rotation + 1) % 4;

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
    ASSERT_PIECE(player.piece);
    ASSERT_ROTATION(player.rotation);

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
    ASSERT_PIECE(player.piece);
    ASSERT_ROTATION(player.rotation);

    for (int i = 0; i < 4; i++)
    {
        int x = player.x + rotations[player.piece][player.rotation][i].x;
        int y = player.y + rotations[player.piece][player.rotation][i].y;
        field_draw_cell(x, y, player.c);
    }
}