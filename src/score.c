#include <math.h>

#include "score.h"
#include "utils.h"

static long score;
static int total_lines;

void score_init(void)
{
    score = 0;
    total_lines = 0;
}

void score_add(BONUS bonus)
{
    // https://tetris.fandom.com/wiki/Scoring#Guideline_scoring_system
    switch (bonus)
    {
    case SINGLE:
        score += level_get() * 100;
        total_lines += 1;
        break;
    case DOUBLE:
        score += level_get() * 300;
        total_lines += 2;
        break;
    case TRIPLE:
        score += level_get() * 500;
        total_lines += 3;
        break;
    case TETRIS:
        score += level_get() * 800;
        total_lines += 4;
        break;
    default:
        safe_exit("Invalid bonus", 1);
        break;
    }
}

long score_get(void)
{
    return score;
}

int level_get(void)
{
    // return 20;
    return total_lines / 10 + 1;
}

int lines_cleared_get(void)
{
    return total_lines;
}

int lines_until_next_level(void)
{
    return level_get() * 10 - total_lines;
}

double gravity_get(void)
{
    // https://tetris.fandom.com/wiki/Tetris_Worlds#Gravity
    return pow(0.8 - ((double)(min(level_get(), 15) - 1) * 0.007), (double)(min(level_get(), 15) - 1));
}