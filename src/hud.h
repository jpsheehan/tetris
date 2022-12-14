#ifndef HUD_H_
#define HUD_H_

#include <stdbool.h>

void hud_init(void);
void hud_deinit(void);
void hud_update(void);
void hud_draw(bool show_minos);

#endif