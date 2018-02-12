#include <epoxy/gl.h>
#include <epoxy/glx.h>

#include "globaldefs.h"
#include "stringlib.h"	//for idlist


#include "filesys.h"	//for file ops

//#include "iqm.h"	//for iqm handling


#include "hashtables.h"	//for idlist
#include "mathlib.h"	//for bbox stuff currently

#include "matrixlib.h"
#include "entitymanager.h"


#include "glmanager.h"	//for checkglerror
IDLIST_INTERNAL(entity, entity_t, entitylist_t);

int entity_init(void){
	IDLIST_INIT(entity, entity_t, entitylist_t);
	entity_ok = TRUE;
	return TRUE;
}
int entity_unload(entity_t *e){
//TODO
	return TRUE;
}
IDLIST_CODE(entity, entity_t, entitylist_t);
