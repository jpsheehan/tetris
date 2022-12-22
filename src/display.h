#ifndef DISPLAY_H
#define DISPLAY_H

#include <allegro5/allegro5.h>

#define BUFFER_W 320
#define BUFFER_H 240

void disp_init(void);
void disp_update(ALLEGRO_EVENT* pEvent);
void disp_pre_draw(void);
void disp_post_draw(void);
void disp_register_event_source(ALLEGRO_EVENT_QUEUE* queue);
int disp_get_buffer_width(void);
int disp_get_buffer_height(void);
ALLEGRO_BITMAP* disp_get_buffer(void);

#endif