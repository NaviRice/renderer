#include <stdio.h>
#include "globaldefs.h"

#include "cvarmanager.h" //todo actually make use of this

#include "stringlib.h" //todo?


#include "navincludes.h"
#include <dlfcn.h>


#include <pthread.h> //for thread safety






//junk exposed to the nav DLL

//todo put this stuff ina  neat little struct

//todo figure out if i want this entirely locked or not
pthread_mutex_t nav_next_lock;
navwaypoint_t nav_next = {0};

//todo make this better


int nav_setCurrentWaypoint(navwaypoint_t * n){

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
	return TRUE;
}


















navgcallheader_t *ng = 0;

static void * nav_lib;

navecallheader_t ne = {0};

int nav_ok;


void sys_unloadNavAPI(void){ //todo move to sys
	if(nav_lib) dlclose(nav_lib);
	nav_lib = 0;
}

navgcallheader_t * sys_getNavAPI(navecallheader_t *ne){ //todo move to sys

	if(nav_lib){ printf("NAV/sys_getNavAPI: ERROR nav DLL still open, cant reload\n"); return FALSE;}
	nav_lib = dlopen("./navcode.so", RTLD_NOW);

	if(!nav_lib){ printf("NAV/sys_getNavAPI: ERROR opening navcode.so, file not found\n"); return FALSE;}

	void *(*dllSetupCallbacks) (void*);
	dllSetupCallbacks = (void *) dlsym (nav_lib, "setupNavCodeCallbacks");
	if(!dllSetupCallbacks){ printf("NAV/sys_getNavAPI: ERROR opening navcode.so, setupNavCodeCallbacks not found\n"); sys_unloadNavAPI(); return FALSE;}

	return dllSetupCallbacks(ne);
}

int sys_setupNavCallbacks(void){
//	ne.whatever = whatever;
	ne.nav_setCurrentWaypoint = nav_setCurrentWaypoint;

	ng = sys_getNavAPI(&ne);
	if(!ng){
		printf("NAV/sys_setupNavCallbacks: ERROR could not load nav code????\n");
		return FALSE;
	}
	if(ng->apiver != NAVCODEINCLUDEVERSION){
		printf("NAV/sys_setupNavCallbacks: ERROR NAV API VER %i does not match dll api ver %i\n", NAVCODEINCLUDEVERSION, ng->apiver);
		sys_unloadNavAPI();
		return FALSE;
	}
	if(!ng->initnav){
		printf("NAV/sys_setupNavCallbacks: ERROR nav does not have an initnav function\n");
		sys_unloadNavAPI();
		return FALSE;
	}
	//todo
	return TRUE;
}


int nav_init(void){
	if(!sys_setupNavCallbacks()){
		printf("NAV/nav_init: MAJOR WARNING unable to setup DLL stuffs for %s, defaulting to shitty init\n", "todo set this string?");
	} else if(ng && ng->initnav) ng->initnav();

	nav_ok = TRUE;
	return TRUE;
}




