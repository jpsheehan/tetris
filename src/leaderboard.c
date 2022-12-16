#include "leaderboard.h"

static bool does_leaderboard_file_exist(void);

static char* leaderboard_filename = "leaderboard.dat";

void leaderboard_init(void);

void leaderboard_read_high_scores(MODE mode, int n, SCORE* buffer);

bool leaderboard_is_worthy(MODE mode, SCORE* score)
{
  if (!does_leaderboard_file_exist) {
    return true;
  }

  // TODO
  
  return false;
}

void leaderboard_add_score(MODE mode, SCORE* score);

void leaderboard_save(void);