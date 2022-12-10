#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <allegro5/allegro5.h>

void keyboard_init();
void keyboard_update(ALLEGRO_EVENT *event);
void keyboard_register_event_source(ALLEGRO_EVENT_QUEUE* queue);
bool keyboard_is_pressed(int keycode);
bool keyboard_is_just_pressed(int keycode);

#endif