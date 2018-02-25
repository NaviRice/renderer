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
	//go through alloced memory, free it
	entitymemitem_t *next;
	entitymemitem_t *m = e->mem.next; //avoid freeing the static one lel
	while(m){
		next = m->next;
		//dont bother memsetting lel
		free(m);
		m = next;
	}
	//all done freeing alloced memory
	return TRUE;
}

void entity_markForDeletion(entity_t *e){
	//todo do this better elohel
	e->gonnadie = 1;
}


void * entity_memAlloc(entity_t *e, size_t size){
	entitymemitem_t *m = malloc(size + sizeof(entitymemitem_t));
	m->size = size;
	m->next = e->mem.next;
	m->prev = &e->mem;
	return m+1;
}

//CAN NOT CALL ON A VOID, unlike usual realloc
void * entity_memRealloc(void * ptr, size_t newsize){
	//get header
	entitymemitem_t *m = ((entitymemitem_t *)ptr)-1;

	m = realloc(m, newsize + sizeof(entitymemitem_t));
	//since "m" is now different, update the next and previous ll items to point to it
	if(m->prev) m->prev->next = m;
	if(m->next) m->next->prev = m;
	m->size = newsize;
	return m+1;
}

void entity_memFree(void *ptr){
	//get header
	entitymemitem_t *m = ((entitymemitem_t *)ptr)-1;
	//remove from doubly linked list
	if(m->next) m->next->prev = m->prev;
	//should generally always have a prev, but we do it anyway just in case
	if(m->prev) m->prev->next = m->next;
	//free my soul

	//todo double check that we cant get a loop case?
	//i really think its impossible unless someone does suck fuck
	free(m);
}

IDLIST_CODE(entity, entity_t, entitylist_t);
