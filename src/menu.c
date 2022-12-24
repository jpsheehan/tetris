#include <stdlib.h>
#include <allegro5/allegro_font.h>

#include "menu.h"
#include "utils.h"
#include "asset_loader.h"
#include "input.h"
#include "resources.h"

int option_font = 0;
int title_font = 0;

static ALLEGRO_FONT *load_option_font(void);
static ALLEGRO_FONT *load_title_font(void);

void menu_init(void)
{
    if (option_font == 0)
    {
        option_font = asset_loader_load("option font", A_FONT, (AssetLoaderCallback)&load_option_font);
    }

    if (title_font == 0)
    {
        title_font = asset_loader_load("title font", A_FONT, (AssetLoaderCallback)&load_title_font);
    }
}

void menu_draw(MENU *menu)
{
    al_draw_text(A(title_font), al_map_rgb_f(1, 1, 1), menu->x, menu->y, ALLEGRO_ALIGN_CENTER, menu->title);

    for (int i = 0; i < menu->n_opts; i++)
    {
        char *option = menu->opts[i];
        ALLEGRO_COLOR color = i == menu->idx ? al_map_rgb_f(1, 0, 0) : al_map_rgb_f(1, 1, 1);

        al_draw_text(A(option_font), color, menu->x, menu->y + 60 + 30 * i, ALLEGRO_ALIGN_CENTER, option);
    }
}

void menu_update(MENU *menu, ALLEGRO_EVENT *event)
{
    if (event->type == ALLEGRO_EVENT_KEY_DOWN)
    {
        if (event->keyboard.keycode == input_get_mapping(INPUT_MENU_UP))
        {
            if (menu->idx > 0)
                menu->idx--;
        }
        else if (event->keyboard.keycode == input_get_mapping(INPUT_MENU_DOWN))
        {
            if (menu->idx + 1 < menu->n_opts)
                menu->idx++;
        }
        else if (event->keyboard.keycode == input_get_mapping(INPUT_MENU_SELECT))
        {
            if (menu->callback == NULL)
                safe_exit("Menu callback cannot be NULL", 1);
            menu->callback(menu->idx);
        }
        else if (event->keyboard.keycode == input_get_mapping(INPUT_MENU_BACK))
        {

            if (menu->callback == NULL)
                safe_exit("Menu callback cannot be NULL", 1);
            menu->callback(-1);
        }
    }
}

static ALLEGRO_FONT *load_option_font(void)
{
    return al_load_font(R_FONT_XOLONIUM_REGULAR, REL(1), 0);
}

static ALLEGRO_FONT *load_title_font(void)
{
    return al_load_font(R_FONT_XOLONIUM_REGULAR, REL(1.33), 0);
}