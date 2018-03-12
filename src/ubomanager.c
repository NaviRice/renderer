#include <epoxy/gl.h>
#include "globaldefs.h"
#include "contextmanager.h" //i dont think i need this

#include "ubomanager.h"

#include "glmanager.h"

GLint ubo_maxsize;
GLint ubo_alignment;


IDLIST_INTERNAL_NOHASH(ubo, ubo_t);

int ubo_init(void){
	IDLIST_INIT_NOHASH(ubo, ubo_t);
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &ubo_maxsize);
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &ubo_alignment);
	printf("Max uniform size is %i\n", ubo_maxsize);
	printf("Uniform alignment size is %i\n", ubo_alignment);
	return TRUE;
}


int ubo_setup(ubo_t *u){
	if(!u) return 0;
	#if UBOPINGPONG > 1
		glGenBuffers(UBOPINGPONG, u->id);
	#else
		glGenBuffers(1, &u->id);
	#endif
	u->type = 1;
	return TRUE;
}


int ubo_bind(ubo_t *u){
	if(!u->type){
		printf("binding a ubo that isnt setup yet\n");
		ubo_setup(u);
	}
	#if UBOPINGPONG > 1
		glBindBuffer(GL_UNIFORM_BUFFER, u->id[u->pingplace]);
	#else
		glBindBuffer(GL_UNIFORM_BUFFER, u->id);
	#endif
	return TRUE;
}

int ubo_unload(ubo_t *u){
	#if UBOPINGPONG > 1
		glDeleteBuffers(UBOPINGPONG, u->id);
	#else
		glDeleteBuffers(1, &u->id);
	#endif
	if(u->data)free(u->data);
	return TRUE;
}


int ubo_pushData(ubo_t *u, const unsigned int size, const void * data){
	if(!size || !data) return -1;
	unsigned int mysize = (size + ubo_alignment-1) & ~(ubo_alignment -1);
	unsigned int place = u->place;
	unsigned int newsize = place+mysize;
	if(newsize > u->size){
		u->data = realloc(u->data, newsize);
		u->size = newsize;
	}
	memcpy(u->data + place, data, size);
	u->place = newsize;
	return place;
}




int ubo_pushDataNoAlign(ubo_t *u, const unsigned int size, const void * data){
	if(!size || !data) return -1;
	unsigned int place = u->place;
	unsigned int newsize = place+size;
	if(newsize > u->size){
		u->data = realloc(u->data, newsize);
		u->size = newsize;
	}
	memcpy(u->data  + place, data, size);
	u->place = newsize;
	return place;
}


int ubo_flushData(ubo_t *u){
	unsigned int place = u->place;
	if(!place) return FALSE;
	#if UBOPINGPONG > 1
		u->pingplace = (u->pingplace +1) % UBOPINGPONG;
	#endif
	ubo_bind(u);
	glBufferData(GL_UNIFORM_BUFFER, place, u->data, GL_DYNAMIC_DRAW);
	u->place = 0;
	return TRUE;
}


IDLIST_CODE_NOHASH(ubo, ubo_t);
