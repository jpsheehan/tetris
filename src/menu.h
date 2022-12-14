#ifndef MENU_H_
#define MENU_H_

#include <allegro5/allegro.h>

#define MENU_MAX_OPTS 5

typedef struct MENU {
    char* title;
    int x, y;
    int idx;
    int n_opts;
    char* opts[MENU_MAX_OPTS];
    void (*callback)(int);
} MENU;

void menu_init(void);
void menu_draw(MENU* menu);
void menu_update(MENU* menu, ALLEGRO_EVENT* event);

#endif