#include <stdint.h>

#include "main_menu.h"
#include "asset_loader.h"
#include "menu.h"
#include "display.h"
#include "utils.h"
#include "tetris.h"
#include "transition.h"

#define MENU_TRANSITION_TIME_S 0.20

typedef enum STATE
{
    TRANS_TO_MAIN,
    TRANS_FROM_MAIN,
    MAIN,
    TRANS_TO_ARCADE,
    TRANS_FROM_ARCADE,
    ARCADE,
    TRANS_TO_OPTIONS,
    TRANS_FROM_OPTIONS,
    OPTIONS,
    MENU_CALLBACK,
#if DEBUG_MENU
    DEBUG,
    DEBUG_LINE_CLEAR,
    DEBUG_PERFECT_CLEAR,
    DEBUG_TSPIN,
    DEBUG_TSPIN_MINI,
#endif
    STATE_MAX,
} STATE;

static void main_callback(int idx);
static void arcade_callback(int idx);
static void options_callback(int idx);
#if DEBUG_MENU
static void debug_callback(int idx);
static void debug_line_clear_callback(int idx);
static void debug_perfect_clear_callback(int idx);
static void debug_tspin_callback(int idx);
static void debug_tspin_mini_callback(int idx);
#endif
static void transition_callback(void);

static void *transition = NULL;
static STATE state = TRANS_TO_MAIN;
static STATE next_state = STATE_MAX;
static void (*callback)(int);
static CHOICE game_mode = CHOICE_MAX;

static MENU main_menu = {
    .title = "Tetris",
    .n_opts =
#if DEBUG_MENU
        4,
#else
        3,
#endif
    .opts = {
        "Arcade",
        "Options",
        "Quit",
#if DEBUG_MENU
        "Debug",
#endif
    },
    .x = BUFFER_W / 2,
    .y = 50,
    .idx = 0,
    .callback = &main_callback,
};

static MENU arcade_menu = {
    .title = "Arcade",
    .n_opts = 5,
    .opts = {
        "Marathon",
        "Sprint",
        "Ultra",
        "Endless",
        "Back",
    },
    .x = BUFFER_W / 2,
    .y = 50,
    .idx = 0,
    .callback = &arcade_callback,
};

static MENU options_menu = {
    .title = "Options",
    .n_opts = 1,
    .opts = {
        "Back"},
    .x = BUFFER_W / 2,
    .y = 50,
    .idx = 0,
    .callback = &options_callback,
};

#if DEBUG_MENU
static MENU debug_menu = {
    .title = "Debug",
    .n_opts = 5,
    .opts = {
        "Line Clear",
        "Perfect Clear",
        "T-Spin",
        "T-Spin Mini",
        "Back",
    },
    .x = BUFFER_W / 2,
    .y = 50,
    .idx = 0,
    .callback = &debug_callback,
};
static MENU debug_line_clear_menu = {
    .title = "Debug: Line Clear",
    .n_opts = 5,
    .opts = {
        "Single",
        "Double",
        "Triple",
        "Tetris",
        "Back",
    },
    .x = BUFFER_W / 2,
    .y = 50,
    .idx = 0,
    .callback = &debug_line_clear_callback,
};
static MENU debug_perfect_clear_menu = {
    .title = "Debug: Perfect Clear",
    .n_opts = 5,
    .opts = {
        "Single",
        "Double",
        "Triple",
        "Tetris",
        "Back",
    },
    .x = BUFFER_W / 2,
    .y = 50,
    .idx = 0,
    .callback = &debug_perfect_clear_callback,
};
static MENU debug_tspin_menu = {
    .title = "Debug: T-Spin",
    .n_opts = 4,
    .opts = {
        "T-Spin Single",
        "T-Spin Double",
        "T-Spin Triple",
        "Back",
    },
    .x = BUFFER_W / 2,
    .y = 50,
    .idx = 0,
    .callback = &debug_tspin_callback,
};
static MENU debug_tspin_mini_menu = {
    .title = "Debug: T-Spin Mini",
    .n_opts = 2,
    .opts = {
        "T-Spin Mini Single",
        "Back",
    },
    .x = BUFFER_W / 2,
    .y = 50,
    .idx = 0,
    .callback = &debug_tspin_mini_callback,
};
#endif

void main_menu_init(void (*cb)(CHOICE))
{
    menu_init();
    state = TRANS_TO_MAIN;
    callback = (void (*)(int))cb;
    transition = transition_start(FADE_IN, MENU_TRANSITION_TIME_S, &transition_callback);
}

void main_menu_update(ALLEGRO_EVENT *pEvent)
{
    switch (state)
    {
    case MAIN:
        menu_update(&main_menu, pEvent);
        break;
    case ARCADE:
        menu_update(&arcade_menu, pEvent);
        break;
    case OPTIONS:
        menu_update(&options_menu, pEvent);
        break;
#if DEBUG_MENU
    case DEBUG:
        menu_update(&debug_menu, pEvent);
        break;
    case DEBUG_LINE_CLEAR:
        menu_update(&debug_line_clear_menu, pEvent);
        break;
    case DEBUG_PERFECT_CLEAR:
        menu_update(&debug_perfect_clear_menu, pEvent);
        break;
    case DEBUG_TSPIN:
        menu_update(&debug_tspin_menu, pEvent);
        break;
    case DEBUG_TSPIN_MINI:
        menu_update(&debug_tspin_mini_menu, pEvent);
        break;
#endif
    case TRANS_FROM_ARCADE:
    case TRANS_FROM_MAIN:
    case TRANS_FROM_OPTIONS:
    case TRANS_TO_ARCADE:
    case TRANS_TO_MAIN:
    case TRANS_TO_OPTIONS:
        transition_update(transition, pEvent);
        break;
    default:
        safe_exit("Invalid menu state", 1);
        break;
    }
}

void main_menu_draw(void)
{
    switch (state)
    {
    case MAIN:
        menu_draw(&main_menu);
        break;
    case ARCADE:
        menu_draw(&arcade_menu);
        break;
    case OPTIONS:
        menu_draw(&options_menu);
        break;
#if DEBUG_MENU
    case DEBUG:
        menu_draw(&debug_menu);
        break;
    case DEBUG_LINE_CLEAR:
        menu_draw(&debug_line_clear_menu);
        break;
    case DEBUG_PERFECT_CLEAR:
        menu_draw(&debug_perfect_clear_menu);
        break;
    case DEBUG_TSPIN:
        menu_draw(&debug_tspin_menu);
        break;
    case DEBUG_TSPIN_MINI:
        menu_draw(&debug_tspin_mini_menu);
        break;
#endif
    case TRANS_FROM_ARCADE:
    case TRANS_TO_ARCADE:
        menu_draw(&arcade_menu);
        transition_draw(transition);
        break;
    case TRANS_FROM_MAIN:
    case TRANS_TO_MAIN:
        menu_draw(&main_menu);
        transition_draw(transition);
        break;
    case TRANS_FROM_OPTIONS:
    case TRANS_TO_OPTIONS:
        menu_draw(&options_menu);
        transition_draw(transition);
        break;
    default:
        safe_exit("Invalid menu state in draw", 1);
        break;
    }
}

void main_callback(int idx)
{
    switch (idx)
    {
    case 0:
        // Arcade
        state = TRANS_FROM_MAIN;
        next_state = ARCADE;
        arcade_menu.idx = 0;
        transition = transition_start(FADE_OUT, MENU_TRANSITION_TIME_S, &transition_callback);
        break;
    case 1:
        // Options
        state = TRANS_FROM_MAIN;
        next_state = OPTIONS;
        options_menu.idx = 0;
        transition = transition_start(FADE_OUT, MENU_TRANSITION_TIME_S, &transition_callback);
        break;
    case -1:
    case 2:
        // Quit
        callback(CHOICE_EXIT);
        break;
#if DEBUG_MENU
    case 3:
        state = DEBUG;
        debug_menu.idx = 0;
        break;
#endif
    default:
        safe_exit("Invalid main menu index", 1);
        break;
    }
}

void arcade_callback(int idx)
{
    state = TRANS_FROM_ARCADE;
    switch (idx)
    {
    case 0:
        game_mode = CHOICE_MARATHON;
        next_state = MENU_CALLBACK;
        break;
    case 1:
        game_mode = CHOICE_SPRINT;
        next_state = MENU_CALLBACK;
        break;
    case 2:
        game_mode = CHOICE_ULTRA;
        next_state = MENU_CALLBACK;
        break;
    case 3:
        game_mode = CHOICE_ENDLESS;
        next_state = MENU_CALLBACK;
        break;
    case -1:
    case 4:
        next_state = MAIN;
        break;
    default:
        safe_exit("Invalid arcade menu index", 1);
        break;
    }
    transition = transition_start(FADE_OUT, MENU_TRANSITION_TIME_S, &transition_callback);
}

void options_callback(int idx)
{
    switch (idx)
    {
    case -1:
    case 0:
        state = TRANS_FROM_OPTIONS;
        next_state = MAIN;
        transition = transition_start(FADE_OUT, MENU_TRANSITION_TIME_S, &transition_callback);
        break;
    default:
        safe_exit("Invalid options menu index", 1);
        break;
    }
}

#if DEBUG_MENU

#include "field.h"
#include "randomiser.h"
#include "game.h"

typedef enum DEBUG_TEST
{
    D_LINE_CLEAR_SINGLE,
    D_LINE_CLEAR_DOUBLE,
    D_LINE_CLEAR_TRIPLE,
    D_LINE_CLEAR_TETRIS,
    D_PERFECT_CLEAR_SINGLE,
    D_PERFECT_CLEAR_DOUBLE,
    D_PERFECT_CLEAR_TRIPLE,
    D_PERFECT_CLEAR_TETRIS,
    D_TSPIN_SINGLE,
    D_TSPIN_DOUBLE,
    D_TSPIN_TRIPLE,
    D_TSPIN_MINI_SINGLE,
} DEBUG_TEST;

static PIECE debug_bag[][7] = {
    {
        // line clear single
        I,
        I,
        I,
        I,
        I,
        I,
        I,
    },
    {
        // line clear double
        I,
        I,
        I,
        I,
        I,
        I,
        I,
    },
    {
        // line clear triple
        I,
        I,
        I,
        I,
        I,
        I,
        I,
    },
    {
        // line clear tetris
        I,
        I,
        I,
        I,
        I,
        I,
        I,
    },
    {
        // perfect clear single
        I,
        I,
        I,
        I,
        I,
        I,
        I,
    },
    {
        // perfect clear double
        O,
        O,
        O,
        O,
        O,
        O,
        O,
    },
    {
        // perfect clear triple
        J,
        J,
        J,
        J,
        J,
        J,
        J,
    },
    {
        // perfect clear tetris
        I,
        I,
        I,
        I,
        I,
        I,
        I,
    },
    {
        // T-Spin Single
        T,
        T,
        T,
        T,
        T,
        T,
        T,
    },
    {
        // T-Spin Double
        T,
        T,
        T,
        T,
        T,
        T,
        T,
    },
    {
        // T-Spin Triple
        T,
        T,
        T,
        T,
        T,
        T,
        T,
    },
    {
        // T-Spin Mini Single
        T,
        T,
        T,
        T,
        T,
        T,
        T,
    },
};

/* NOTE: These fields are being described upside-down and mirrored */
static uint16_t debug_fields[][20] = {
    {
        // line clear single
        0b1111011111,
    },
    {
        // line clear double
        0b1111011111,
        0b1111011111,
    },
    {
        // line clear triple
        0b1111011111,
        0b1111011111,
        0b1111011111,
    },
    {
        // line clear tetris
        0b1111011111,
        0b1111011111,
        0b1111011111,
        0b1111011111,
        0b1111011111,
    },
    {
        // perfect clear single
        0b1100001111,
    },
    {
        // perfect clear double
        0b1111001111,
        0b1111001111,
    },
    {
        // perfect clear triple
        0b1111011111,
        0b1111011111,
        0b1110011111,
    },
    {
        // perfect clear tetris
        0b1111011111,
        0b1111011111,
        0b1111011111,
        0b1111011111,
    },
    {
        // T-Spin Single
        0b1111011111,
        0b1110001110,
        0b1111001111,
    },
    {
        // T-Spin Double,
        0b1111111001,
        0b1111111001,
        0b1111111101,
        0b0000000001,
        0b0000000011,
    },
    {
        // T-Spin Triple
        0b1111111101,
        0b1111111001,
        0b1111111101,
        0b1111110001,
        0b1111110011,
    },
    {
        // T-Spin Mini Single
        0b1111101111,
        0b0011000100,
        0b0001100000,
    },
};

void debug_load_custom_level(DEBUG_TEST test)
{
    randomiser_seed(7, debug_bag[test]);

    field_init();
    uint16_t *field = debug_fields[test];
    ALLEGRO_COLOR c = al_map_rgb(0xea, 0xba, 0xbe);
    for (int y = 0; y < FIELD_H; y++)
    {
        for (int x = 0; x < FIELD_W; x++)
        {
            if (field[y] & (1 << (FIELD_W - x - 1)))
                field_set_used_safely(FIELD_W - x - 1, FIELD_H - y - 1, c);
        }
    }

    callback(CHOICE_DEBUG);
}

void debug_callback(int idx)
{
    switch (idx)
    {
    case 4:
    case -1:
        state = MAIN;
        debug_menu.idx = 0;
        break;
    case 0:
        // line clears
        state = DEBUG_LINE_CLEAR;
        break;
    case 1:
        // perfect clears
        state = DEBUG_PERFECT_CLEAR;
        break;
    case 2:
        // t-spins
        state = DEBUG_TSPIN;
        break;
    case 3:
        // t-spin minis
        state = DEBUG_TSPIN_MINI;
        break;
    default:
        safe_exit("Invalid debug menu index", 1);
        break;
    }
}

void debug_line_clear_callback(int idx)
{
    switch (idx)
    {
    case 4:
    case -1:
        state = DEBUG;
        break;
    case 0:
        // single
        debug_load_custom_level(D_LINE_CLEAR_SINGLE);
        break;
    case 1:
        // double
        debug_load_custom_level(D_LINE_CLEAR_DOUBLE);
        break;
    case 2:
        // triple
        debug_load_custom_level(D_LINE_CLEAR_TRIPLE);
        break;
    case 3:
        // tetris
        debug_load_custom_level(D_LINE_CLEAR_TETRIS);
        break;
    default:
        safe_exit("Invalid debug line clear menu index", 1);
        break;
    }
}

void debug_perfect_clear_callback(int idx)
{
    switch (idx)
    {
    case 4:
    case -1:
        state = DEBUG;
        debug_perfect_clear_menu.idx = 0;
        break;
    case 0:
        // single
        debug_load_custom_level(D_PERFECT_CLEAR_SINGLE);
        break;
    case 1:
        // double
        debug_load_custom_level(D_PERFECT_CLEAR_DOUBLE);
        break;
    case 2:
        // triple
        debug_load_custom_level(D_PERFECT_CLEAR_TRIPLE);
        break;
    case 3:
        // tetris
        debug_load_custom_level(D_PERFECT_CLEAR_TETRIS);
        break;
    default:
        safe_exit("Invalid debug perfect clear menu index", 1);
        break;
    }
}

void debug_tspin_callback(int idx)
{
    switch (idx)
    {
    case 3:
    case -1:
        debug_tspin_menu.idx = 0;
        state = DEBUG;
        break;
    case 0:
        // single
        debug_load_custom_level(D_TSPIN_SINGLE);
        break;
    case 1:
        // double
        debug_load_custom_level(D_TSPIN_DOUBLE);
        break;
    case 2:
        // triple
        debug_load_custom_level(D_TSPIN_TRIPLE);
        break;
    default:
        safe_exit("Invalid debug tspin menu index", 1);
        break;
    }
}

void debug_tspin_mini_callback(int idx)
{
    switch (idx)
    {
    case 1:
    case -1:
        debug_tspin_mini_menu.idx = 0;
        state = DEBUG;
        break;
    case 0:
        // single
        debug_load_custom_level(D_TSPIN_MINI_SINGLE);
        break;
    default:
        safe_exit("Invalid debug tspin-mini menu index", 1);
        break;
    }
}
#endif

static void transition_callback(void)
{
    // printf("Callback started, state = %d, next_state = %d\n", state, next_state);
    switch (state)
    {
    case TRANS_TO_ARCADE:
        state = ARCADE;
        break;
    case TRANS_TO_MAIN:
        state = MAIN;
        break;
    case TRANS_TO_OPTIONS:
        state = OPTIONS;
        break;
    case TRANS_FROM_ARCADE:
    case TRANS_FROM_MAIN:
    case TRANS_FROM_OPTIONS:
        switch (next_state)
        {
        case MAIN:
            state = TRANS_TO_MAIN;
            break;
        case ARCADE:
            state = TRANS_TO_ARCADE;
            break;
        case OPTIONS:
            state = TRANS_TO_OPTIONS;
            break;
        case MENU_CALLBACK:
            state = TRANS_FROM_ARCADE;
            next_state = ARCADE;
            callback(game_mode);
            // transition_free(transition);
            // transition = transition_start(FADE_OUT, MENU_TRANSITION_TIME_S, &transition_callback);
            return;
        default:
            safe_exit("Invalid next_state", 1);
            break;
        }
        transition_free(transition);
        transition = transition_start(FADE_IN, MENU_TRANSITION_TIME_S, &transition_callback);
        break;
    default:
        safe_exit("Invalid non-transition state", 1);
        break;
    }
    // printf("Callback ended, state = %d, next_state = %d\n", state, next_state);
}