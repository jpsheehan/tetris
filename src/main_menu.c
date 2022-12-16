#include "main_menu.h"
#include "asset_loader.h"
#include "menu.h"
#include "display.h"
#include "utils.h"
#include "tetris.h"

typedef enum STATE
{
    MAIN,
    ARCADE,
    OPTIONS,
#if DEBUG_MENU
    DEBUG,
    DEBUG_LINE_CLEAR,
    DEBUG_PERFECT_CLEAR,
// DEBUG_TSPIN,
// DEBUG_TSPIN_MINI,
#endif
} STATE;

void main_callback(int idx);
void arcade_callback(int idx);
void options_callback(int idx);
#if DEBUG_MENU
void debug_callback(int idx);
void debug_line_clear_callback(int idx);
void debug_perfect_clear_callback(int idx);
// void debug_tspin_callback(int idx);
// void debug_tspin_mini_callback(int idx);
#endif

static STATE state = MAIN;
static void (*callback)(int);

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
    .title = "Debug - Line Clear",
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
    .title = "Debug - Perfect Clear",
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
#endif

void main_menu_init(void (*cb)(CHOICE))
{
    menu_init();
    state = MAIN;
    callback = (void (*)(int))cb;
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
#endif
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
#endif
    default:
        safe_exit("Invalid menu state", 1);
        break;
    }
}

void main_callback(int idx)
{
    switch (idx)
    {
    case 0:
        // Arcade
        state = ARCADE;
        arcade_menu.idx = 0;
        break;
    case 1:
        // Options
        state = OPTIONS;
        options_menu.idx = 0;
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
    switch (idx)
    {
    case 0:
        callback(CHOICE_MARATHON);
        break;
    case 1:
        callback(CHOICE_SPRINT);
        break;
    case 2:
        callback(CHOICE_ULTRA);
        break;
    case 3:
        callback(CHOICE_ENDLESS);
        break;
    case -1:
    case 4:
        state = MAIN;
        break;
    default:
        safe_exit("Invalid arcade menu index", 1);
        break;
    }
}

void options_callback(int idx)
{
    switch (idx)
    {
    case -1:
    case 0:
        state = MAIN;
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
};

static u_int16_t debug_fields[][20] = {
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
};

void debug_load_custom_level(DEBUG_TEST test)
{
    randomiser_seed(7, debug_bag[test]);

    field_init();
    u_int16_t* field = debug_fields[test];
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
        break;
    case 3:
        // t-spin minis
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
        safe_exit("Invalid debug menu index", 1);
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
        safe_exit("Invalid debug menu index", 1);
        break;
    }
}
#endif