#include <stdlib.h>
#include <allegro5/allegro_font.h>

#include "menu.h"
#include "utils.h"
#include "asset_loader.h"

ALLEGRO_FONT *font = NULL;

void menu_init(void)
{
    if (font == NULL)
    {
        font = asset_loader_load(A_FONT, (AssetLoaderCallback)&al_create_builtin_font);
        must_init(font, "menu font");
    }
}

void menu_draw(MENU *menu)
{
    int x = menu->x - al_get_text_width(font, menu->title) / 2;
    al_draw_text(font, al_map_rgb_f(1, 1, 1), x, menu->y, 0, menu->title);

    for (int i = 0; i < menu->n_opts; i++)
    {
        char *option = menu->opts[i];
        ALLEGRO_COLOR color = i == menu->idx ? al_map_rgb_f(1, 0, 0) : al_map_rgb_f(1, 1, 1);
        x = menu->x - al_get_text_width(font, option) / 2;

        al_draw_text(font, color, x, menu->y + 30 + 20 * i, 0, option);
    }
}

void menu_update(MENU *menu, ALLEGRO_EVENT *event)
{
    if (event->type == ALLEGRO_EVENT_KEY_DOWN)
    {
        switch (event->keyboard.keycode)
        {
        case ALLEGRO_KEY_UP:
            if (menu->idx > 0)
                menu->idx--;
            break;
        case ALLEGRO_KEY_DOWN:
            if (menu->idx + 1 < menu->n_opts)
                menu->idx++;
            break;
        case ALLEGRO_KEY_ENTER:
            // do something
            if (menu->callback == NULL)
                safe_exit("Menu callback cannot be NULL", 1);
            menu->callback(menu->idx);
            break;
        case ALLEGRO_KEY_ESCAPE:
            if (menu->callback == NULL)
                safe_exit("Menu callback cannot be NULL", 1);
            menu->callback(-1);
            break;
        }
    }
}