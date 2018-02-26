//local includes
#include "../navdllincludeme.h"
#include "nav.h"


navgcallheader_t ng = {0};
navecallheader_t *ne;
int initnav(void){
	//todo
	return TRUE; // todo error che
}


navgcallheader_t* setupNavCodeCallbacks(navecallheader_t *myne){
	ne = myne;
	if(!ne) return FALSE;
	ng.apiver = NAVCODEINCLUDEVERSION;
	ng.initnav = initnav;
	return &ng;
}

