#ifndef RANDOMISER_7BAG_H_
#define RANDOMISER_7BAG_H_

#include "player.h"

void randomiser_init(void);

void randomiser_deinit(void);

/**
 * Advances the randomiser, returning the next piece.
*/
PIECE randomiser_next(void);

/**
 * Peeks the next 1 - 6 pieces.
*/
void randomiser_peek(int* n, PIECE* buffer);

#endif