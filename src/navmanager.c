#include <stdio.h>
#include "globaldefs.h"

#include "cvarmanager.h" //todo actually make use of this

#include "stringlib.h" //todo?


#include "navincludes.h"
#include <dlfcn.h>


#include <pthread.h> //for thread safety






int (*nav_lib_init)(void) = 0;
int (*nav_lib_setNextWaypointCallback)( int (*n)(navwaypoint_t *n)) = 0;

pthread_mutex_t nav_next_lock;
navwaypoint_t nav_next = {0};

//todo make this better


int nav_setNextWaypoint(navwaypoint_t * n){

	if(!n)return FALSE;

	pthread_mutex_lock(&nav_next_lock);

	//make sure to dealloc old ones
	if(nav_next.desc)free(nav_next.desc);
	if(nav_next.icon)free(nav_next.icon);

	nav_next = *n;
	//todo check if strdup returns null on a null input
	if(nav_next.desc) nav_next.desc = strdup(nav_next.desc);
	if(nav_next.icon) nav_next.icon = strdup(nav_next.icon);
	pthread_mutex_unlock(&nav_next_lock);


	printf("Nav waypoint update %f %f %s %s\n", nav_next.x, nav_next.y, nav_next.desc, nav_next.icon);
	return TRUE;
}



















static void * nav_lib;


int nav_ok;


void sys_unloadNavAPI(void){ //todo move to sys
	if(nav_lib) dlclose(nav_lib);
	nav_lib = 0;
	nav_lib_init = 0;
	nav_lib_setNextWaypointCallback = 0;
}

int sys_getNavAPI(void){ //todo move to sys

	if(nav_lib){ printf("NAV/sys_getNavAPI: ERROR nav DLL still open, cant reload\n"); return FALSE;}
	nav_lib = dlopen("./navcode.so", RTLD_NOW);

	if(!nav_lib){ printf("NAV/sys_getNavAPI: ERROR opening navcode.so, file not found\n"); return FALSE;}


	nav_lib_init = dlsym(nav_lib, "nav_init");
	nav_lib_setNextWaypointCallback = dlsym(nav_lib, "nav_setNextWaypointCallback");

	if(!nav_lib_init){ printf("NAV/sys_getNavAPI: ERROR opening navcode.so, nav_init not found\n"); sys_unloadNavAPI(); return FALSE;}
	if(!nav_lib_setNextWaypointCallback){ printf("NAV/sys_getNavAPI: ERROR opening navcode.so, nav_setNextWaypointCallback not found\n"); sys_unloadNavAPI(); return FALSE;}

	return TRUE;
}

int nav_init(void){
	if(!sys_getNavAPI()){
		printf("NAV/nav_init: ERROR unable to setup DLL stuffs for %s\n", "todo set this string?");
		nav_ok = FALSE;
		return FALSE;
	}
	if(!nav_lib_init()){
		printf("NAV/nav_init: ERROR nav_lib_init failed\n");
		sys_unloadNavAPI();
		nav_ok = FALSE;
		return FALSE;
	}

	if(nav_lib_setNextWaypointCallback)nav_lib_setNextWaypointCallback(nav_setNextWaypoint);


	nav_ok = TRUE;
	return TRUE;
}




