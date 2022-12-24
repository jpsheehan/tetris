#include <allegro5/allegro_primitives.h>

#include "display.h"
#include "utils.h"
#include "asset_loader.h"

#define DISP_W (scaling_factor * (BUFFER_W))
#define DISP_H (scaling_factor * (BUFFER_H))

int disp = 0;
int buffer = 0;
int logo = 0;

bool fullscreen = false;
float scaling_factor = 1.0;

static void set_scaling_factor(int width, int height);

static ALLEGRO_DISPLAY *create_display(void)
{
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    al_set_new_window_title("Tetris");

    return al_create_display(DISP_W, DISP_H);
}

static ALLEGRO_BITMAP *create_buffer(void)
{
    return al_create_bitmap(BUFFER_W, BUFFER_H);
}

static ALLEGRO_BITMAP *load_logo(void)
{
    return al_load_bitmap("./resources/graphics/logo.png");
}

void disp_init(void)
{
    if (disp == 0)
    {
        disp = asset_loader_load("display", A_DISPLAY, (AssetLoaderCallback)&create_display);

        al_set_window_constraints(A(disp), BUFFER_W, BUFFER_H, 0, 0);
    }

    if (buffer == 0)
    {
        buffer = asset_loader_load("buffer", A_BITMAP, (AssetLoaderCallback)&create_buffer);
    }

    if (logo == 0)
    {
        logo = asset_loader_load("logo", A_BITMAP, (AssetLoaderCallback)&load_logo);
        al_set_display_icon(A(disp), A(logo));
    }
}

void disp_update(ALLEGRO_EVENT *pEvent)
{
    switch (pEvent->type)
    {
    case ALLEGRO_EVENT_DISPLAY_RESIZE:
        set_scaling_factor(pEvent->display.width, pEvent->display.height);
        break;
    case ALLEGRO_EVENT_KEY_DOWN:
        if (pEvent->keyboard.keycode == ALLEGRO_KEY_F11)
        {
            fullscreen = !fullscreen;
            al_set_display_flag(A(disp), ALLEGRO_FULLSCREEN_WINDOW, fullscreen);
        }
        break;
    }
}

void disp_pre_draw(void)
{
    al_set_target_bitmap(A(buffer));
}

void disp_post_draw(void)
{
    float dx = (al_get_display_width(A(disp)) - DISP_W) / 2;
    float dy = (al_get_display_height(A(disp)) - DISP_H) / 2;

    al_set_target_backbuffer(A(disp));
    al_draw_filled_rectangle(0, 0, al_get_display_width(A(disp)), al_get_display_height(A(disp)), al_map_rgb_f(0, 0, 0));
    al_draw_scaled_bitmap(A(buffer), 0, 0, BUFFER_W, BUFFER_H, dx, dy, DISP_W, DISP_H, 0);
    al_flip_display();
}

void disp_register_event_source(ALLEGRO_EVENT_QUEUE *queue)
{
    al_register_event_source(queue, al_get_display_event_source(A(disp)));
}

ALLEGRO_BITMAP *disp_get_buffer(void)
{
    return A(buffer);
}

static void set_scaling_factor(int width, int height)
{
    float width_ratio = (float)width / (float)BUFFER_W;
    float height_ratio = (float)height / (float)BUFFER_H;
    scaling_factor = MIN(width_ratio, height_ratio);
    al_acknowledge_resize(A(disp));
}