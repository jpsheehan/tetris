#ifndef MAIN_MENU_H_
#define MAIN_MENU_H_

#include <allegro5/allegro5.h>

typedef enum CHOICE {
    CHOICE_MARATHON,
    CHOICE_SPRINT,
    CHOICE_ULTRA,
    CHOICE_ENDLESS,
    CHOICE_EXIT,
} CHOICE;

void main_menu_init(void (*cb)(CHOICE));
void main_menu_update(ALLEGRO_EVENT* pEvent);
void main_menu_draw(void);

#endif
