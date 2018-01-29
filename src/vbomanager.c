#include <epoxy/gl.h>
#include "globaldefs.h"
#include "contextmanager.h" //only for context_current;
#include "vbomanager.h"
#include "glmanager.h"



IDLIST_INTERNAL_NOHASH(vbo, vbo_t);


int vbo_init(void){
	IDLIST_INIT_NOHASH(vbo, vbo_t);
	return TRUE;
}


int vbo_setup(vbo_t *vbo){
	int i;
	if(!vbo) return 0;

	switch(vbo->type){
		case 1:
			//generate buffers
			for(i =0; i< MAXATTRIBS; i++){
				if(!vbo->datawidth[i])continue;
				glGenBuffers(1, &vbo->vertsid[i]);
				if(!vbo->vertsid[i]){
					printf("VBO/vbo_setup error glGenBuffers returned 0 for attrib %i\n", i);
					return vbo->type;
				}
			}
			glGenBuffers(1, &vbo->facesid);
			vbo->type = 2;
		case 2:
			//set up attribs (for this context)
			if(!vbo->vaoid[context_current]) glGenVertexArrays(1, &vbo->vaoid[context_current]);
			glBindVertexArray(vbo->vaoid[context_current]);	//will eventually put this into a state
			for(i = 0; i < MAXATTRIBS; i++){
				GLuint swidth = vbo->datawidth[i];
				if(!swidth) continue;
				glBindBuffer(GL_ARRAY_BUFFER, vbo->vertsid[i]);
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(i, swidth, GL_FLOAT, GL_FALSE, 0, NULL); //todo other types, such as glubyte
			}
			//cant forget the faces buffer!
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo->facesid);
//			vbo->type = 3;
//so i can re-do that code for each context
		default:
			return vbo->type;
	}
}


int vbo_bind(vbo_t *vbo){
	if(!vbo->vaoid[context_current]){
	//set it up now i guess
		printf("binding a vao that isnt setup yet\n");
		vbo_setup(vbo);
	}
	glBindVertexArray(vbo->vaoid[context_current]);
	return TRUE;
}


int vbo_unload(vbo_t *v){
	glDeleteBuffers(MAXATTRIBS, v->vertsid);
	glDeleteBuffers(1, &v->facesid);
	int i;
	for(i=0; i < NUMCONTEXTS; i++){
		if(v->vaoid[i])	glDeleteVertexArrays(1, &v->vaoid[i]);
	}
	return TRUE;
}


IDLIST_CODE_NOHASH(vbo, vbo_t);
