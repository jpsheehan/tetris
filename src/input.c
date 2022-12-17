#include <allegro5/allegro.h>

#include "input.h"
#include "tetris.h"
#include "utils.h"

#define ASSERT_INPUT(i) ASSERT_RANGE(i, 0, INPUT_MAX, "input")

int input_mappings[] = {
    ALLEGRO_KEY_UP,
    ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_ESCAPE,
    ALLEGRO_KEY_ENTER,
    ALLEGRO_KEY_ESCAPE,
#if USE_GUIDELINE_INPUTS
    ALLEGRO_KEY_LEFT,
    ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_SPACE,
    ALLEGRO_KEY_C,
    ALLEGRO_KEY_UP,
    ALLEGRO_KEY_Z,
#else
    ALLEGRO_KEY_LEFT,
    ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_UP,
    ALLEGRO_KEY_SPACE,
    ALLEGRO_KEY_X,
    ALLEGRO_KEY_Z,
#endif
};

int input_get_mapping(INPUT i)
{
    ASSERT_INPUT(i);

    return input_mappings[i];
}