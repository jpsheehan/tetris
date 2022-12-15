#include "display.h"
#include "utils.h"
#include "asset_loader.h"

ALLEGRO_DISPLAY* disp = NULL;
ALLEGRO_BITMAP* buffer = NULL;

static ALLEGRO_DISPLAY* create_display(void)
{
    return al_create_display(DISP_W, DISP_H);
}

static ALLEGRO_BITMAP* create_buffer(void)
{
    return al_create_bitmap(BUFFER_W, BUFFER_H);
}

void disp_init()
{
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    disp = asset_loader_load(A_DISPLAY, (void*(*)(void))&create_display);
    must_init(disp, "display");

    buffer = asset_loader_load(A_BITMAP, (void*(*)(void))&create_buffer);
    must_init(buffer, "bitmap buffer");
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

void disp_register_event_source(ALLEGRO_EVENT_QUEUE* queue)
{
    al_register_event_source(queue, al_get_display_event_source(disp));
}

ALLEGRO_BITMAP* disp_get_buffer(void)
{
    return buffer;
}