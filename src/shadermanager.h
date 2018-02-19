#ifndef SHADERNANAGERHEADER
#define SHADERMANAGERHEADER
typedef struct shadersource_s {
	char * filename;
	char * data;
	char * helperstring;
	int length;
	struct shadersource_s * next; //linked listy
}shadersource_t;

typedef struct shadercvar_s {
	int id; //cvar id
	//todo type
	char * name;
	char * sbuf;
	struct shadercvar_s *next;
} shadercvar_t;

typedef struct shader_s {
	int type;//todo figure out types
	int myid;
	char * name;
	unsigned int programid; //todo move?
	unsigned int vertid;	//todo move?
	unsigned int fragid;
	unsigned int geomid;
	shadersource_t * vsources;
	shadersource_t * fsources;
	shadersource_t * gsources;

	shadercvar_t *cvars;

	//temporary
	int uniloc[16];
	int uniblock[2];
} shader_t;

typedef struct shaderlist_s { //todo move these into idlist?
	shader_t **list;
	int count;
} shaderlist_t;

#include "idlist.h"
//DO NOT USE _remove manually unless you are SURE about it!
IDLIST_HEADER(shader, shader_t, shaderlist_t);

int shader_init(void);

int shader_register(const char * name);

int shader_load(shader_t *s);
int shader_unload(shader_t *s);

int shader_shutdown(void);


#endif
