#ifndef SCORE_H_
#define SCORE_H_

void score_init(void);
void score_deinit(void);
void score_increase(int points);
long score_get(void);
int level_get(void);

#endif