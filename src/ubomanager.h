#ifndef UBOMANAGERHEADER
#define UBOMANAGERHEADER


typedef struct ubo_s {
	int myid;
	unsigned int place;
	unsigned int size;
	GLubyte *data;
	char type;
#if UBOPINGPONG > 1
	unsigned int pingplace;
	GLuint id [UBOPINGPONG];
#else
	GLuint id;
#endif

} ubo_t;




#include "idlist.h"

IDLIST_HEADER_NOHASH(ubo, ubo_t);

int ubo_init(void);

int ubo_bind(ubo_t *u);
int ubo_unload(ubo_t *u);

#endif
