#ifndef SCORE_H_
#define SCORE_H_

#include "game.h"

void score_init(void);
void score_deinit(void);
void score_add(BONUS bonus);

long score_get(void);
int level_get(void);
int lines_cleared_get(void);
int lines_until_next_level(void);
double gravity_get(int level);

#endif