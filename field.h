#ifndef FIELD_H_
#define FIELD_H_

#include <allegro5/allegro.h>
#include "display.h"

#define FIELD_W 10
#define FIELD_H 20

#define CELL_W 10
#define CELL_H 10

#define FIELD_MARGIN_X ((BUFFER_W - FIELD_W * CELL_W) / 2)
#define FIELD_MARGIN_Y ((BUFFER_H - FIELD_H * CELL_H) / 2)

typedef struct CELL
{
    ALLEGRO_COLOR c;
    bool used;
} CELL;

void field_init(void);
void field_draw(void);
void field_draw_cell(int x, int y, ALLEGRO_COLOR c);
bool field_get_used_or_default(int x, int y, bool default_);
void field_set_used_safely(int x, int y, ALLEGRO_COLOR c);

#endif