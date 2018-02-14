//global includes
#include <stdio.h>
//local includes
#include "globaldefs.h"
#include "matrixlib.h"
//#include "texturemanager.h"
#include "contextmanager.h"
#include "vbomanager.h"
#include "modelmanager.h"
//#include "shadermanager.h"
//#include "physics.h"
#include "entitymanager.h"
//#include "worldmanager.h"
#include "viewportmanager.h"
//#include "lightmanager.h"
#include "gamecodemanager.h"

#include "mathlib.h"
//#include "console.h"

#include "cvarmanager.h"
#include "filesys.h"
#include "stringlib.h"
//#include "gamecodeincludes.h"

#include <dlfcn.h> //todo move to sys

int gamecode_ok;
int gamecode_tGameTime = 0;

/*
gcallheader_t *gc = 0;
static void * game_lib;
ecallheader_t ec;

void sys_unloadGameAPI(void){ // todo move to sys
	if(game_lib)dlclose(game_lib);
	game_lib = 0;
}

gcallheader_t * sys_getGameAPI(ecallheader_t *ec){ //todo move to sys
	if(game_lib){ console_printf("Error: game DLL still open, cant reload\n"); return FALSE;}


	//todo search for it, cvar it, etc
	game_lib = dlopen("./gamecode.so", RTLD_NOW);

	if(!game_lib){ console_printf("Error opening gamecode.so, file not found\n"); return FALSE;}
	void *(*dllSetupCallbacks) (void*);
	dllSetupCallbacks = (void *) dlsym (game_lib, "setupGameCodeCallbacks");
	if(!dllSetupCallbacks){
		console_printf("Error opening gamecode.so\n");
		sys_unloadGameAPI();
		return FALSE;
	}
	return dllSetupCallbacks(ec);
}



int setupGameCodeCallbacks(void){
	ec.console_printf = console_printf;
	ec.console_nprintf = console_nprintf;

	ec.cvar_register = cvar_register;
	ec.cvar_unregister = cvar_unregister;
	ec.cvar_nameset = cvar_nameset;
	ec.cvar_pset = cvar_pset;
	ec.cvar_idset = cvar_idset;
	ec.cvar_returnById = cvar_returnById;
	ec.cvar_findByNameRPOINT = cvar_findByNameRPOINT;
	ec.cvar_findByNameRINT = cvar_findByNameRINT;

	ec.entity_findByNameRPOINT = entity_findByNameRPOINT;
	ec.entity_findByNameRINT = entity_findByNameRINT;
	ec.entity_findAllByNameRPOINT = entity_findAllByNameRPOINT;
	ec.entity_findAllByNameRINT = entity_findAllByNameRINT;
	ec.entity_returnById = entity_returnById;
	ec.entity_addRPOINT = entity_addRPOINT;
	ec.entity_addRINT = entity_addRINT;
	ec.entity_delete = entity_delete;

	ec.file_loadString = file_loadString;
	ec.file_loadStringNoLength = file_loadStringNoLength;

	ec.light_addRINT = light_addRINT;
	ec.light_addRPOINT = light_addRPOINT;

	ec.shader_createAndAddRINT = shader_createAndAddRINT;

	ec.string_toVec = string_toVec;

	ec.texture_createAndAddGroupRINT = texture_createAndAddGroupRINT;

	ec.model_createAndAddRINT = model_createAndAddRINT;


	gc = sys_getGameAPI(&ec);
	if(!gc){
		console_printf("Error: could not load game code\n");
		return FALSE;
	}
	//todo implement a new api checking formula, maybe ranges of compatible
	if(gc->apiver != GAMECODEINCLUDEVERSION){
		console_printf("Error: Gamecode version is %i, engine version is %i, not compatible\n", gc->apiver, GAMECODEINCLUDEVERSION);
		return FALSE;
	}
	if(!gc->initgame){
		console_printf("Error: Gamecode does not have an initgame function\n");
		return FALSE;
	}
	//todo
	return TRUE;
}

*/
extern int calcEntAttachMat(entity_t *e);
extern int recalcEntBBox(entity_t *e);

int gamecode_init(void){
/*
#ifdef ODE_COMPILE
	physics_init();
#endif

*/
	if(!entity_init()){
		gamecode_ok = FALSE;
		return FALSE;
	}
/*
	if(!setupGameCodeCallbacks()){
		gamecodeOK = FALSE;
		return FALSE; //todo something
	}

	loadWorld("world");
	loadWorld("world2");

	gc->initgame();
*/


	//temp till i get seperated gamecode
	entity_t e = {0};
	e.name = strdup("teapot");
	e.pos[2] = -2.0;
	e.angle[2] = -90.0;
	e.anglevel[1] = 20.0;
	e.needsmatupdate = 1;
	e.scale = 2.0;
//	e.type = 1;
	e.modelid = model_register("models/teapot.iqm");
	entity_t *teapot = entity_addRPOINT(e);

	e.name = strdup("bunny ass bitch");
	e.pos[2] = -1.5;
	e.angle[2] = -90.0;
	e.anglevel[1] = -90.0;
	e.needsmatupdate = 1;
	e.scale = 0.02;
	e.attachmentid = teapot->myid;
//	e.type = 1;
	e.modelid = model_register("models/bunny.iqm");
	entity_t *bunny = entity_addRPOINT(e);

	entity_pruneList();
	gamecode_ok = TRUE;
	return TRUE; // todo error check
}
int recalcEntBBox(entity_t * e){
	model_t * m = model_returnById(e->modelid);
	if(!m) return FALSE;
		Matrix4x4_Transformsimdu(&e->mat, &m->bboxp[0], &e->bboxp[0]);

		e->bbox[0] = e->bbox[1] =  e->bboxp[0];
		e->bbox[2] = e->bbox[3] =  e->bboxp[1];
		e->bbox[4] = e->bbox[5] =  e->bboxp[2];
	int i;
	for(i = 1; i < 8; i++){
		int oneplace = i*3;
		Matrix4x4_Transformsimdu(&e->mat, &m->bboxp[oneplace], &e->bboxp[oneplace]);

		if(e->bboxp[oneplace] > e->bbox[0]) e->bbox[0] = e->bboxp[oneplace];
		else if(e->bboxp[oneplace] < e->bbox[1]) e->bbox[1] = e->bboxp[oneplace];
		if(e->bboxp[oneplace+1] > e->bbox[2]) e->bbox[2] = e->bboxp[oneplace+1];
		else if(e->bboxp[oneplace+1] < e->bbox[3]) e->bbox[3] = e->bboxp[oneplace+1];
		if(e->bboxp[oneplace+2] > e->bbox[4]) e->bbox[4] = e->bboxp[oneplace+2];
		else if(e->bboxp[oneplace+2] < e->bbox[5]) e->bbox[5] = e->bboxp[oneplace+2];
	}
	//now clamp to sphere size
	//assumes finalpos is set
	vec3_t maxs, mins;
	vec3add(maxs, e->finalpos, m->bsphere);
	vec3sub(mins, e->finalpos, m->bsphere);


	//note, no elses here, because it can easily be both
	if(e->bbox[0] > maxs[0]) e->bbox[0] = maxs[0];
	if(e->bbox[1] < mins[0]) e->bbox[1] = mins[0];
	if(e->bbox[2] > maxs[1]) e->bbox[2] = maxs[1];
	if(e->bbox[3] < mins[1]) e->bbox[3] = mins[1];
	if(e->bbox[4] > maxs[2]) e->bbox[4] = maxs[2];
	if(e->bbox[5] < mins[2]) e->bbox[5] = mins[2];


	e->needsbboxupdate = FALSE;


//	if(!e->leaf) addEntityToWorld(e);
//	else moveEntInWorld(e);
	return TRUE;
}

int calcEntAttachMat(entity_t * e){ //return value is weather e->mat got changed
	if(!e->myid) return FALSE;
	if(e->attachmentid){
		entity_t * attacher = entity_returnById(e->attachmentid);
		if(!attacher){
			 e->attachmentid = 0;
		}
		else if (calcEntAttachMat(attacher)){ //dat recursion
			matrix4x4_t tempmat;
//			Matrix4x4_CreateFromQuakeEntity(&tempmat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale);
//			Matrix4x4_CreateFromQuakeEntity(&tempmat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale/attacher->scale);
			Matrix4x4_CreateFromQuakeEntity(&tempmat, e->pos[0]/attacher->scale, e->pos[1]/attacher->scale, e->pos[2]/attacher->scale, e->angle[0], e->angle[1], e->angle[2], e->scale/attacher->scale);

			Matrix4x4_Concatsimdu(&e->mat, &attacher->mat, &tempmat);
			e->needsmatupdate = 2;
			return TRUE;
		}
		//todo figure this out...
		else if (e->needsmatupdate & 1){
//		else if (TRUE){
			e->needsmatupdate = 2;
			if(attacher){
				matrix4x4_t tempmat;
	//			Matrix4x4_CreateFromQuakeEntity(&tempmat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale);
//				Matrix4x4_CreateFromQuakeEntity(&tempmat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale/attacher->scale);
				Matrix4x4_CreateFromQuakeEntity(&tempmat, e->pos[0]/attacher->scale, e->pos[1]/attacher->scale, e->pos[2]/attacher->scale, e->angle[0], e->angle[1], e->angle[2], e->scale/attacher->scale);
				Matrix4x4_Concatsimdu(&e->mat, &attacher->mat, &tempmat);

				return TRUE;
			} else {
				Matrix4x4_CreateFromQuakeEntity(&e->mat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale);
				return TRUE;
			}
		}
		else if (e->needsmatupdate) return TRUE;
		//else implied
		return FALSE;
	} else if (e->needsmatupdate & 1) {
		Matrix4x4_CreateFromQuakeEntity(&e->mat, e->pos[0], e->pos[1], e->pos[2], e->angle[0], e->angle[1], e->angle[2], e->scale);
		e->needsmatupdate = 2;
		return TRUE;
	} else if(e->needsmatupdate) return TRUE;
	return FALSE;
}
/*
void entityCollideBBoxL(entity_t * e, worldleaf_t *l){
		int * elist = l->entlist;
		int max = l->entityarraylasttaken+1;
		int i;
		for(i = 0; i < max; i++){
			int cei = elist[i];
			if(!cei) continue;
			entity_t * ce = entity_returnById(cei);
			//check if they collide
			if(testBBoxInBBox(e->bbox, ce->bbox)){
				//run collide function
				e->touch(e, cei);
//				e->touch(e, ce);
			}
		}
		worldleaf_t ** children = l->children;
		int j;
		for(j = 0; j < 4; j++){
			if(children[j] && (children[j]->includes & WORLDTREEENTITY) && testBBoxInBBox(e->bbox, children[j]->bbox))
				entityCollideBBoxL(e, children[j]);
		}
}

//same as entityCollideBBoxL, but it checks to make sure it doesnt collide against itself
void entityCollideBBoxSL(entity_t * e, worldleaf_t *l){
		int myid = e->myid;
		int * elist = l->entlist;
		int max = l->entityarraylasttaken+1;
		int i;
		for(i = 0; i < max; i++){
			int cei = elist[i];
			if(!cei || cei == myid) continue;
			entity_t * ce = entity_returnById(cei);
			//check if they collide
			if(testBBoxInBBox(e->bbox, ce->bbox)){
				//run collide function
				e->touch(e, cei);
//				e->touch(e, ce);
			}
		}
		worldleaf_t ** children = l->children;
		int j;
		for(j = 0; j < 4; j++){
			if(children[j] && (children[j]->includes & WORLDTREEENTITY) && testBBoxInBBox(e->bbox, children[j]->bbox))
				entityCollideBBoxL(e, children[j]);
		}
}

void entityCollideBBox(entity_t *e){
	//caller of this function should make sure e has a ontouch func defined
	worldleaf_t * l = e->leaf;
	worldleaf_t * upleaf;
	//entity will always collide with all its parent leafs, so test them all
	for(upleaf = l->parent; upleaf; upleaf = upleaf->parent){
		if(!(upleaf->myincludes & WORLDTREEENTITY))continue; // may be unnecisary
		int * elist = upleaf->entlist;
		int max = upleaf->entityarraylasttaken+1;
		int i;
		for(i = 0; i < max; i++){
			int cei = elist[i];
			if(!cei) continue;
			entity_t * ce = entity_returnById(cei);
			//check if they collide
			if(testBBoxInBBox(e->bbox, ce->bbox)){
				//run collide function
				e->touch(e, cei);
//				e->touch(e, ce);
			}
		}
	}
	//now check children leafs
	entityCollideBBoxSL(e, l);
}
*/
void gamecode_tick(void){ //todo maybe change to float in seconds
	gamecode_tGameTime+=GCTIMESTEP;
	int i;

	//ent phys
	for(i = 0; i <= entity_arraylasttaken; i++){
		entity_t * e = &entity_list[i];
		if(!e->myid) continue;
#ifdef ODE_COMPILE
		if(e->phys.movetype == ODEDYNAMIC){
			if(physics_getEntD(e)) e->needsmatupdate = TRUE;
		} else {
#endif
			if(e->vel[0] || e->vel[1] || e->vel[2]){

				e->pos[0] += e->vel[0] * GCTIMESTEPSECONDS;
				e->pos[1] += e->vel[1] * GCTIMESTEPSECONDS;
				e->pos[2] += e->vel[2] * GCTIMESTEPSECONDS;
				e->needsmatupdate = TRUE;
			}
			if(e->anglevel[0] || e->anglevel[1] || e->anglevel[2]){
				e->angle[0] += e->anglevel[0] * GCTIMESTEPSECONDS;
				e->angle[1] += e->anglevel[1] * GCTIMESTEPSECONDS;
				e->angle[2] += e->anglevel[2] * GCTIMESTEPSECONDS;
				e->needsmatupdate = TRUE;
			}
		}
#ifdef ODE_COMPILE
	}
#endif
	for(i = 0; i <= entity_arraylasttaken; i++){
		entity_t *e = &entity_list[i];
		if(!e->myid) continue;
		//crazy recursive attachment system
		calcEntAttachMat(e);
		//was it updated in the process?
		if(e->needsmatupdate || e->needsbboxupdate){
			if(e->attachmentid){
				//todo update for finalmatrix
				Matrix4x4_OriginFromMatrix(&e->mat, e->finalpos);
			} else {
				e->finalpos[0] = e->pos[0];
				e->finalpos[1] = e->pos[1];
				e->finalpos[2] = e->pos[2];
			}
			recalcEntBBox(e);
		}
	}
	//todo maybe convert to an entity "carry" system instead of a light attach system
//	lightLoop();
	for(i = 0; i <= entity_arraylasttaken; i++){// make sure they dont update again
		entity_list[i].needsmatupdate = FALSE;
		entity_list[i].needsbboxupdate = FALSE;
	}


//		if(e->think && e->nextthink <= gamecode_tGameTime){
//			e->think();
//		}


	//ent gamecode

}