#ifndef FIELD_H_
#define FIELD_H_

#include <stdbool.h>
#include <allegro5/allegro.h>
#include "display.h"
#include "mino.h"

#define FIELD_W 10
#define FIELD_H 20

#define FIELD_MARGIN_X ((BUFFER_W - FIELD_W * MINO_W) / 2)
#define FIELD_MARGIN_Y ((BUFFER_H - FIELD_H * MINO_H) / 2)

typedef struct CELL
{
    ALLEGRO_COLOR c;
    bool used;
} CELL;

void field_init(void);
void field_update(void);
void field_draw(bool show_stack);
void field_draw_mino(PIECE piece, int rotation, int offset_x, int offset_y, ALLEGRO_COLOR c);
bool field_get_used_or_default(int x, int y, bool default_);
void field_set_used_safely(int x, int y, ALLEGRO_COLOR c);

#endif