#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include "globaldefs.h"
#include "vbomanager.h"



IDLIST_INTERNAL_NOHASH(vbo, vbo_t);


int vbo_init(void){
	IDLIST_INIT_NOHASH(vbo, vbo_t);
	return TRUE;
}


int vbo_setup(vbo_t *vbo){
	if(!vbo) return 0;

	switch(vbo->type){
		case 1:
			//generate buffers
			glGenVertexArrays(1, &vbo->vaoid);
			int i;
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
			//set up attribs
			glBindVertexArray(vbo->vaoid);		//will eventually put this into a state
			for(i = 0; i < MAXATTRIBS; i++){
				GLuint swidth = vbo->datawidth[i];
				if(!swidth) continue;
				glBindBuffer(GL_ARRAY_BUFFER, vbo->vertsid[i]);
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(i, swidth, GL_FLOAT, GL_FALSE, 0, NULL); //todo other types, such as glubyte
			}
			vbo->type = 3;
		default:
			return vbo->type;
	}
}


int vbo_unload(vbo_t *v){
	glDeleteBuffers(MAXATTRIBS, v->vertsid);
	glDeleteBuffers(1, &v->facesid);
	glDeleteVertexArrays(1, &v->vaoid);
	return TRUE;
}


IDLIST_CODE_NOHASH(vbo, vbo_t);
