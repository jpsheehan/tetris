#include "score.h"

static long score;
static int level;

void score_init(void)
{
    score = 0;
    level = 1;
}

void score_increase(int delta)
{
    score += delta;
}

long score_get(void)
{
    return score;
}

int level_get(void)
{
    return level;
}