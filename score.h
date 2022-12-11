#ifndef SCORE_H_
#define SCORE_H_

void score_init(void);
void score_deinit(void);
void score_add_cleared_lines(int lines_cleared);

long score_get(void);
int level_get(void);
int lines_cleared_get(void);
int lines_until_next_level(void);
double gravity_get(void);

#endif