#include "keyboard.h"
#include "utils.h"

#define KEY_SEEN 1
#define KEY_RELEASED 2
unsigned char key[ALLEGRO_KEY_MAX];

void keyboard_init()
{
    must_init(al_install_keyboard(), "keyboard");
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
    if (keycode <= 0 || keycode >= ALLEGRO_KEY_MAX)
        return false;
    return key[keycode] == KEY_SEEN;
}

bool keyboard_is_just_pressed(int keycode)
{
    if (keycode <= 0 || keycode >= ALLEGRO_KEY_MAX)
        return false;
    return key[keycode] == (KEY_SEEN | KEY_RELEASED);
}

#undef KEY_SEEN
#undef KEY_RELEASED