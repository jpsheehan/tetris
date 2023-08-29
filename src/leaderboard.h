#ifndef LEADERBOARD_H_
#define LEADERBOARD_H_

#include <stdint.h>
#include <stdbool.h>

#include "game.h"

#define MAX_USERNAME_CHARACTERS 5
#define MAX_LEADERBOARD_ENTRIES 8

typedef struct SCORE {
  MODE mode;
  char name[MAX_USERNAME_CHARACTERS + 1];
  uint64_t value;
} SCORE;

size_t leaderboard_read_high_scores(MODE mode, size_t n, SCORE* buffer);
bool leaderboard_is_worthy(MODE mode, uint64_t value);
void leaderboard_add_score(MODE mode, const char* name, uint64_t value);
void leaderboard_dump(void);

#endif