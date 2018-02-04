#ifndef ENTITYMANAGERHEADER
#define ENTITYMANAGERHEADER
typedef struct entity_s {
	int type;//todo figure out types
	int myid;
	char * name;

	vec3_t pos;
	vec3_t angle;
	float scale;
	//used to generate
	matrix4x4_t mat;
	//which is further used to generate
	matrix4x4_t final;
	int needsmatupdate;

	//and finally generates this
	vec_t bboxp[24];
	vec_t bbox[6];



	int modelid; //todo multiple models per ent? thoughts about that


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


#endif
