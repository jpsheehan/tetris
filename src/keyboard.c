#include "keyboard.h"
#include "utils.h"

#define KEY_SEEN 1
#define KEY_RELEASED 2
#define ASSERT_KEYCODE(keycode) ASSERT_RANGE(keycode, 0, ALLEGRO_KEY_MAX, "keycode");

unsigned char key[ALLEGRO_KEY_MAX];

void keyboard_init()
{
    memset(key, 0, sizeof(key));
}

void keyboard_update(ALLEGRO_EVENT *event)
{
    switch (event->type)
    {
    case ALLEGRO_EVENT_TIMER:
        for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
            key[i] &= KEY_SEEN;
        break;
    case ALLEGRO_EVENT_KEY_DOWN:
        key[event->keyboard.keycode] = (KEY_SEEN | KEY_RELEASED);
        break;
    case ALLEGRO_EVENT_KEY_UP:
        key[event->keyboard.keycode] &= KEY_RELEASED;
        break;
    }
}

void keyboard_register_event_source(ALLEGRO_EVENT_QUEUE* queue)
{
    al_register_event_source(queue, al_get_keyboard_event_source());
}

bool keyboard_is_pressed(int keycode)
{
    ASSERT_KEYCODE(keycode);
    return key[keycode] == KEY_SEEN;
}

bool keyboard_is_just_pressed(int keycode)
{
    ASSERT_KEYCODE(keycode);
    return key[keycode] == (KEY_SEEN | KEY_RELEASED);
}

void keyboard_reset_key(int keycode)
{
    ASSERT_KEYCODE(keycode);
    key[keycode] &= KEY_RELEASED;
}

#undef KEY_SEEN
#undef KEY_RELEASED
#undef ASSERT_KEYCODE