#include "score.h"

static long score;
static int total_lines;

void score_init(void)
{
    score = 0;
    total_lines = 0;
}

void score_add_cleared_lines(int lines_cleared)
{
    // https://tetris.fandom.com/wiki/Scoring#Guideline_scoring_system
    switch (lines_cleared) {
        case 1:
            score += lines_cleared * 100;
            break;
        case 2:
            score += lines_cleared * 300;
            break;
        case 3:
            score += lines_cleared * 500;
            break;
        case 4:
            score += lines_cleared * 800;
            break;
        default:
            return;
    }

    total_lines += lines_cleared;
}

long score_get(void)
{
    return score;
}

int level_get(void)
{
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