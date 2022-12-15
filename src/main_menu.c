#include "main_menu.h"
#include "asset_loader.h"
#include "menu.h"
#include "display.h"
#include "utils.h"

typedef enum STATE
{
    MAIN,
    ARCADE,
    OPTIONS,
} STATE;

void main_callback(int idx);
void arcade_callback(int idx);
void options_callback(int idx);

static STATE state = MAIN;
static void (*callback)(int);

static MENU main_menu = {
    .title = "Tetris",
    .n_opts = 3,
    .opts = {
        "Arcade",
        "Options",
        "Quit"},
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