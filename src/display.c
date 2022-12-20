#include "display.h"
#include "utils.h"
#include "asset_loader.h"

int disp = 0;
int buffer = 0;
int logo = 0;

static ALLEGRO_DISPLAY *create_display(void)
{
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

void disp_init()
{
    if (disp == 0)
    {
        al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
        al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

        disp = asset_loader_load("display", A_DISPLAY, (AssetLoaderCallback)&create_display);

        al_set_window_title(A(disp), "Tetris");
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

void disp_pre_draw()
{
    al_set_target_bitmap(A(buffer));
}

void disp_post_draw()
{
    al_set_target_backbuffer(A(disp));
    al_draw_scaled_bitmap(A(buffer), 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);
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