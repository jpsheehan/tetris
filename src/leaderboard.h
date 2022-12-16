#ifndef LEADERBOARD_H_
#define LEADERBOARD_H_

#include <stdint.h>
#include <stdbool.h>

#include "game.h"

#define LEADERBOARD_MAX_NAME_CHARS 5

typedef struct SCORE {
  char name[LEADERBOARD_MAX_NAME_CHARS + 1];
  uint64_t score;
} SCORE;

void leaderboard_init(void);
void leaderboard_read_high_scores(MODE mode, int n, SCORE* buffer);
bool leaderboard_is_worthy(MODE mode, SCORE* score);
void leaderboard_add_score(MODE mode, SCORE* score);
void leaderboard_save(void);

#endif