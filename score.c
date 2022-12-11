#include "score.h"

static long score;

void score_init(void)
{
    score = 0;
}

void score_increase(int delta)
{
    score += delta;
}

long score_get(void)
{
    return score;
}