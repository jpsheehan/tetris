#include <math.h>

#include "score.h"
#include "utils.h"

static long score;
static int total_lines;

void score_init(void)
{
    score = 0;
    total_lines = 149;
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
    case PERFECT_CLEAR_SINGLE:
        score += level_get() * 800;
        break;
    case PERFECT_CLEAR_DOUBLE:
        score += level_get() * 1200;
        break;
    case PERFECT_CLEAR_TRIPLE:
        score += level_get() * 1800;
        break;
    case PERFECT_CLEAR_TETRIS:
        score += level_get() * 2000;
        break;
    case TSPIN_MINI_SINGLE_NO_LINES:
        score += level_get() * 100;
        break;
    case TSPIN_MINI_SINGLE:
        score += level_get() * 200;
        total_lines += 1;
        break;
    case TSPIN_SINGLE_NO_LINES:
        score += level_get() * 400;
        break;
    case TSPIN_SINGLE:
        score += level_get() * 800;
        total_lines += 1;
        break;
    case TSPIN_DOUBLE:
        score += level_get() * 1200;
        total_lines += 2;
        break;
    case TSPIN_TRIPLE:
        score += level_get() * 1600;
        total_lines += 3;
        break;
    case SOFT_DROP:
        score += level_get() * 1;
        break;
    case HARD_DROP:
        score += level_get() * 2;
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

#define MAX_LEVEL 15

double gravity_get(int level)
{
    // https://tetris.fandom.com/wiki/Tetris_Worlds#Gravity
    return pow(0.8 - ((double)(MIN(level, MAX_LEVEL) - 1) * 0.007), (double)(MIN(level, MAX_LEVEL) - 1));
}