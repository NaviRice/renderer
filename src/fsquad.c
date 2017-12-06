#include <epoxy/gl.h>
#include "glmanager.h"
#include "globaldefs.h"

#include "vbomanager.h"

//todo move to a named one?

vbo_t fsquad_vao = {0};



GLfloat fsquad_posdata[] = {	-1.0, -1.0,
				1.0, -1.0,
				1.0, 1.0,
				-1.0, 1.0};
GLfloat fsquad_tcdata[] = {	0.0, 0.0,
				1.0, 0.0,
				1.0, 1.0,
				0.0, 1.0};

GLuint fsquad_facedata[] = {0, 1, 2, 0, 2, 3};


int fsquad_init(void){
	fsquad_vao.type = 1;
	fsquad_vao.datawidth[0] = 2;
	fsquad_vao.datawidth[2] = 2;
	if(vbo_setup(&fsquad_vao)!=3){
		printf("FSQUAD/fsquad_init error vbo failed to initialize\n");
		return FALSE;
	}
	glBindVertexArray(fsquad_vao.vaoid);
	glBindBuffer(GL_ARRAY_BUFFER, fsquad_vao.vertsid[0]);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), fsquad_posdata, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, fsquad_vao.vertsid[2]);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), fsquad_tcdata, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fsquad_vao.facesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), fsquad_facedata, GL_STATIC_DRAW);
	CHECKGLERROR

	fsquad_vao.numfaces = 2;
	fsquad_vao.numverts = 4;
	return TRUE;
}

int fsquad_render(void){
	glBindVertexArray(fsquad_vao.vaoid);
	glDrawElements(GL_TRIANGLES, fsquad_vao.numfaces*3, GL_UNSIGNED_INT, 0);
	return TRUE;
}
