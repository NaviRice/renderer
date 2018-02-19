#include "../dllincludeme.h"

#include "gc.h"


#include <stdlib.h>

#include "teapot.h"


//todo move spawn shit or init shit or whatever in here



void teapot_randColorThink(entity_t *e){


	e->color[0] = (double)rand()/ (double) RAND_MAX;
	e->color[1] = (double)rand()/ (double) RAND_MAX;
	e->color[2] = (double)rand()/ (double) RAND_MAX;

	//todo i should really figure a way to do this better (+= works)
	e->nextthink =  ec->now + 0.31415;

}
