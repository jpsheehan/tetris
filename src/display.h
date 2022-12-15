#ifndef DISPLAY_H
#define DISPLAY_H

#include <allegro5/allegro5.h>

#define BUFFER_W 320
#define BUFFER_H 240

#define DISP_SCALE 2
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

void disp_init(void);
void disp_pre_draw(void);
void disp_post_draw(void);
void disp_register_event_source(ALLEGRO_EVENT_QUEUE* queue);
ALLEGRO_BITMAP* disp_get_buffer(void);

#endif