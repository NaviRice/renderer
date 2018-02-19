#include "../dllincludeme.h"
#include "gc.h"


#include <stdlib.h>

#include "bunbun.h"


//todo move spawn shit or init shit or whatever in here



void bunbun_randRotThink(entity_t *e){
//	e->nextthink = now; //todo have a now
	e->angle[0] = (double)rand() / (double)RAND_MAX * 360.0;
	e->needsmatupdate |= 1;
//	printf("bunbun is having funfun\n");
	e->nextthink = ec->now + 0.5;

//	printf("ec->now is %f, nextthink is %f\n", ec->now, e->nextthink);
}
