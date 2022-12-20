#include "display.h"
#include "utils.h"
#include "asset_loader.h"

ALLEGRO_DISPLAY *disp = NULL;
ALLEGRO_BITMAP *buffer = NULL;
ALLEGRO_BITMAP *logo = NULL;

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
    if (disp == NULL)
    {
        al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
        al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

        disp = asset_loader_load(A_DISPLAY, (AssetLoaderCallback)&create_display);
        must_init(disp, "display");

        al_set_window_title(disp, "Tetris");
    }

    if (buffer == NULL)
    {
        buffer = asset_loader_load(A_BITMAP, (AssetLoaderCallback)&create_buffer);
        must_init(buffer, "bitmap buffer");
    }

    if (logo == NULL)
    {
        logo = asset_loader_load(A_BITMAP, (AssetLoaderCallback)&load_logo);
        must_init(logo, "program logo");
        al_set_display_icon(disp, logo);
    }
}

void disp_pre_draw()
{
    al_set_target_bitmap(buffer);
}

void disp_post_draw()
{
    al_set_target_backbuffer(disp);
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);
    al_flip_display();
}

void disp_register_event_source(ALLEGRO_EVENT_QUEUE *queue)
{
    al_register_event_source(queue, al_get_display_event_source(disp));
}

ALLEGRO_BITMAP *disp_get_buffer(void)
{
    return buffer;
}