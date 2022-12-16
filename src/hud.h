#ifndef HUD_H_
#define HUD_H_

#include <stdbool.h>

#include "game.h"

typedef struct HUD_UPDATE_DATA
{
    MODE mode;
    int timer_count;
    bool timer_running;
    bool show_minos;
} HUD_UPDATE_DATA;

void hud_init(void);
void hud_update(void);
void hud_draw(HUD_UPDATE_DATA *pData);

#endif