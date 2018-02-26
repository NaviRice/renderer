#include <stdio.h>
#include "globaldefs.h"

#include "cvarmanager.h" //todo actually make use of this

#include "stringlib.h" //todo?


#include "navincludes.h"
#include <dlfcn.h>




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

	ng = sys_getNavAPI(&ne);
	if(!ng){
		printf("NAV/sys_setupNavCallbacks: ERROR could not load nav code????\n");
		return FALSE;
	}
	if(ng->apiver != NAVCODEINCLUDEVERSION){
		printf("NAV/sys_setupNavCallbacks: ERROR could not load nav code????\n");
		sys_unloadNavAPI();
		return FALSE;
	}
	if(!ng->initnav){
		printf("NAV/sys_setupNavCallbacks: ERROR nav does not have an initnav function\n");
		return FALSE;
	}
	//todo
	return TRUE;
}


int nav_init(void){
	if(!sys_setupNavCallbacks()){
		printf("NAV/nav_init: MAJOR WARNING unable to setup DLL stuffs for %s, defaulting to shitty init\n", "todo set this string?");
	}
	if(ng && ng->initnav) ng->initnav();

	nav_ok = TRUE;
	return TRUE;
}
