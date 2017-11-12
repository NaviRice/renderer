#ifndef MODELMANAGERHEADER
#define MODELMANAGERHEADER
typedef struct model_s {
	int type;//todo figure out types
	int myid;
	char * name;
	vbo_t vbo; //not tracked by the idlist

	vec_t bboxp[24];
	vec_t bbox[6];

} model_t;

typedef struct modellist_s {
	model_t **list;
	int count;
} modellist_t;

#include "idlist.h"
//DO NOT USE _remove manually unless you are SURE about it!
IDLIST_HEADER(model, model_t, modellist_t);

int model_init(void);

int model_register(char * name);

int model_load(model_t *m);

int model_unload(model_t *m);

int model_shutdown(void);


#endif
