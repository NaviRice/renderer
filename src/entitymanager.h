#ifndef ENTITYMANAGERHEADER
#define ENTITYMANAGERHEADER
typedef struct entitymemitem_s {
//	void * data;
	size_t size;
	struct entitymemitem_s * next;
	struct entitymemitem_s * prev;
} entitymemitem_t;

typedef struct entity_s {
	int myid;
	char * name;

	vec3_t pos;
	vec3_t vel;

	vec3_t angle;
	vec3_t anglevel;

	float scale;
	float scalevel; //NEW AND IMPROVED
	float finalscale; //after attachment

	int attachmentid;
	//used to generate
	matrix4x4_t mat;
	//which is further used to generate
	//TODO actually set this up!
	matrix4x4_t final;

	//todo actually set up a scale-less mat
	int needsmatupdate;
	int needsbboxupdate; //might be able to get rid of... seems to always appear false

	//and finally generates this
	vec_t bboxp[24];
	vec_t bbox[6];

	vec3_t finalpos; //todo maybe use this?


	void(*remove)(struct entity_s *e);	//called when an entityt is removed (think deconstructor)// currently called AFTER physics, so keep that in mind
	void(*think)(struct entity_s *e);	//called when nextthink <= currenttime
	void(*touch)(struct entity_s *e);	//called on a touch? dont have collision yet //todo
	double nextthink;


	int modelid; //todo multiple models per ent? thoughts about that

	int gonnadie;//todo figure out types

	entitymemitem_t mem; //dont manually use this shit, use the entity_mem* functions

	vec4_t color;


} entity_t;


typedef struct entitylist_s {
	entity_t **list;
	int count;
} entitylist_t;

#include "idlist.h"
//DO NOT USE _remove manually unless you are SURE about it!
IDLIST_HEADER(entity, entity_t, entitylist_t);

int entity_init(void);

int entity_unload(entity_t *e);

int entity_shutdown(void);

//todo
void * entity_memAlloc(entity_t *e, size_t size);
void * entity_memRealloc(void * ptr, size_t size);
void entity_memFree(void * ptr); //todo


//use this to trigger an entity for removal
	//reason why is it goes through the proper process
	//no memory leaks, and calls the entity gamecode remove first
void entity_markForDeletion(entity_t *e);


#endif
