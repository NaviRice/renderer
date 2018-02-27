//local includes
#include "../navdllincludeme.h"
#include "nav.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>


navgcallheader_t ng = {0};
navecallheader_t *ne;
pthread_t worker;
//todo come up a with a deinit



//examp[le
void *navloop(void * ehh){
	while(1){
		sleep(5);

		navwaypoint_t next = {0};
		next.x = (double)rand() * 10.0 / RAND_MAX;
		next.y = (double)rand() * 10.0 / RAND_MAX;
		next.desc = "yolo swag i am a description";
		next.icon = "leftarrow";

		ne->nav_setCurrentWaypoint(&next);
	}
	return NULL;
}


int initnav(void){
	//spawn worker process
	//remember this "init" function i am currently in has to return eventually
	pthread_create(&worker, NULL, &navloop, NULL);

	return TRUE; //should return TRUE on successful init
}


navgcallheader_t* setupNavCodeCallbacks(navecallheader_t *myne){
	ne = myne;
	if(!ne) return FALSE;
	ng.apiver = NAVCODEINCLUDEVERSION;
	ng.initnav = initnav;
	return &ng;
}

