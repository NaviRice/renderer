//local includes
#include "../navincludes.h"
#include "nav.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>


pthread_t worker;
//todo come up a with a deinit

pthread_mutex_t gen_lock;



//todo figure out callbacks

int (*setCurrentWaypointCallback)(navwaypoint_t *p) =0;



//examp[le
void *navloop(void * ehh){
	while(1){
		sleep(5);

		navwaypoint_t next = {0};
		next.x = (double)rand() * 10.0 / RAND_MAX;
		next.y = (double)rand() * 10.0 / RAND_MAX;
		next.desc = "yolo swag i am a description";
		next.icon = "leftarrow";

		//thread safety... a different thread may set these callbacks up.
		pthread_mutex_lock(&gen_lock);
			//if the callback is set... call it!
			if(setCurrentWaypointCallback)setCurrentWaypointCallback(&next);
		pthread_mutex_unlock(&gen_lock);
	}
	return NULL;
}


int nav_setNextWaypointCallback( int (*n)(navwaypoint_t *p)){
	//thread safety... a different thread may use this!
	pthread_mutex_lock(&gen_lock);
		setCurrentWaypointCallback = n;
	pthread_mutex_unlock(&gen_lock);
	return TRUE; // no errors
}

int nav_init(void){
	//remember this "init" function i am currently in has to return soon
	//so i spawn worker thread to handle all the network stuff
	pthread_create(&worker, NULL, &navloop, NULL);

	return TRUE; //should return TRUE on successful init
}
