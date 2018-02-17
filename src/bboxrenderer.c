#include <epoxy/gl.h>
#include <math.h>

#include "globaldefs.h"
#include "glmanager.h"


#include "contextmanager.h"
#include "vbomanager.h"
#include "modelmanager.h"


#include "shadermanager.h"


#include "matrixlib.h"
#include "entitymanager.h"

#include "mathlib.h"
#include "viewportmanager.h"

#include "bboxrenderer.h"

#include "fsquad.h"


int bboxrenderer_lineshader_id = 0;

vbo_t bboxrenderer_bbox_vao = {0};

vbo_t bboxrenderer_bbox_custom_vao = {0};




int bboxrenderer_init(void){
	bboxrenderer_lineshader_id = shader_register("shaders/line.program");
	shader_t *s = shader_returnById(bboxrenderer_lineshader_id);
	shader_load(s);

	bboxrenderer_bbox_vao.type =1; //trick vbo manager into thinking its a legit vbo
	bboxrenderer_bbox_vao.datawidth[0] = 3; //pos
	bboxrenderer_bbox_vao.datawidth[3] = 3; //color

	if(vbo_setup(&bboxrenderer_bbox_vao) != 2){
		printf("BBOXRENDERER/init error vbo setup for bbox lines failed\n");
		return 0;
	}
	int numfaces = 12;
	int numverts = 16;
	//TODO
	float posdata[16*3] = {
		-1.0, -1.0,  -1.0,	1.0, -1.0,  -1.0,	1.0,  1.0,  -1.0,	-1.0,  1.0,  -1.0,
		-1.0, -1.0,  1.0,	1.0, -1.0,  1.0,	1.0,  1.0,  1.0,	-1.0,  1.0,  1.0,
		-1.0, -1.0,  -1.0,	1.0, -1.0,  -1.0,	1.0,  1.0,  -1.0,	-1.0,  1.0,  -1.0,
		-1.0, -1.0,  1.0,	1.0, -1.0,  1.0,	1.0,  1.0,  1.0,	-1.0,  1.0,  1.0 };
	float coldata[16*3] = {
		1.0, 0.0, 0.0,	1.0, 0.0, 0.0,	1.0, 0.0, 0.0,	1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,	0.0, 1.0, 0.0,	0.0, 1.0, 0.0,	0.0, 1.0, 0.0,
		1.0, 1.0, 1.0,	1.0, 1.0, 1.0,	1.0, 1.0, 1.0,	1.0, 1.0, 1.0,
		1.0, 1.0, 1.0,	1.0, 1.0, 1.0,	1.0, 1.0, 1.0,	1.0, 1.0, 1.0 };

	unsigned int indices[12 *2] = {
		0, 1,	1, 2,	2, 3,	3, 0,
		4, 5,	5, 6,	6, 7,	7, 4,
		8,12,	9,13,	10,14,	11,15 };
	vbo_bind(&bboxrenderer_bbox_vao);
	CHECKGLERROR
	glBindBuffer(GL_ARRAY_BUFFER, bboxrenderer_bbox_vao.vertsid[0]);
	glBufferData(GL_ARRAY_BUFFER, numverts * bboxrenderer_bbox_vao.datawidth[0] * sizeof(GLfloat), posdata, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, bboxrenderer_bbox_vao.vertsid[3]);
	//todo update vbomanager to handle ubytes
	glBufferData(GL_ARRAY_BUFFER, numverts * bboxrenderer_bbox_vao.datawidth[3] * sizeof(GLfloat), coldata, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bboxrenderer_bbox_vao.facesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numfaces*2 * sizeof(GLuint), indices, GL_STATIC_DRAW);
	CHECKGLERROR
	bboxrenderer_bbox_vao.numverts = numverts;
	bboxrenderer_bbox_vao.numfaces = numfaces;


	bboxrenderer_bbox_custom_vao.type =1; //trick vbo manager into thinking its a legit vbo
	bboxrenderer_bbox_custom_vao.datawidth[0] = 3; //pos
	bboxrenderer_bbox_custom_vao.datawidth[3] = 3; //color

	if(vbo_setup(&bboxrenderer_bbox_vao) != 2){
		printf("BBOXRENDERER/init error vbo setup for bbox lines failed\n");
		return 0;
	}
	numfaces = 12;
	numverts = 16;
	//TODO
	vbo_bind(&bboxrenderer_bbox_custom_vao);
	CHECKGLERROR
	glBindBuffer(GL_ARRAY_BUFFER, bboxrenderer_bbox_custom_vao.vertsid[0]);
	glBufferData(GL_ARRAY_BUFFER, numverts * bboxrenderer_bbox_custom_vao.datawidth[0] * sizeof(GLfloat), posdata, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, bboxrenderer_bbox_custom_vao.vertsid[3]);
	//todo update vbomanager to handle ubytes
	glBufferData(GL_ARRAY_BUFFER, numverts * bboxrenderer_bbox_custom_vao.datawidth[3] * sizeof(GLfloat), coldata, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bboxrenderer_bbox_custom_vao.facesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numfaces*2 * sizeof(GLuint), indices, GL_STATIC_DRAW);
	CHECKGLERROR
	bboxrenderer_bbox_custom_vao.numverts = numverts;
	bboxrenderer_bbox_custom_vao.numfaces = numfaces;



	return TRUE;
}


int bboxrenderer_renderBBox(viewport_t *v, vec_t bbox[6]){
	if(!v || !v->type){
		printf("BBOXRENDERER/renderBBox ERROR invalid viewport!\n");
		return 0;
	}
	glEnable(GL_DEPTH_TEST);
	vbo_bind(&bboxrenderer_bbox_vao);
	CHECKGLERROR
	shader_t *s = shader_returnById(bboxrenderer_lineshader_id);
	glUseProgram(s->programid);
	matrix4x4_t mat, tmath;
	Matrix4x4_CreateFromQuakeEntity3(&mat, (bbox[0]+bbox[1])*0.5,(bbox[2]+bbox[3])*0.5,(bbox[4]+bbox[5])*0.5, 0.0, 0.0, 0.0, (bbox[0]-bbox[1])*0.5, (bbox[2]-bbox[3])*0.5, (bbox[4]-bbox[5])*0.5);
	Matrix4x4_Concat(&tmath, &v->viewproj, &mat);
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&tmath, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	CHECKGLERROR
	glDrawElements(GL_LINES, bboxrenderer_bbox_vao.numfaces*2, GL_UNSIGNED_INT, 0);
	CHECKGLERROR
	return TRUE;
}
int bboxrenderer_renderBBoxMat(viewport_t *v, matrix4x4_t *mat){
	if(!v || !v->type){
		printf("BBOXRENDERER/renderBBoxMat ERROR invalid viewport!\n");
		return 0;
	}
	glEnable(GL_DEPTH_TEST);
	vbo_bind(&bboxrenderer_bbox_vao);
	CHECKGLERROR
	shader_t *s = shader_returnById(bboxrenderer_lineshader_id);
	glUseProgram(s->programid);
	matrix4x4_t tmath;
	Matrix4x4_Concat(&tmath, &v->viewproj, mat);
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&tmath, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	CHECKGLERROR
	glDrawElements(GL_LINES, bboxrenderer_bbox_vao.numfaces*2, GL_UNSIGNED_INT, 0);
	CHECKGLERROR
	return TRUE;
}
//todo

int bboxrenderer_renderBBoxPCustom(viewport_t *v, vec_t bboxp[24]){
	if(!v || !v->type){
		printf("BBOXRENDERER/renderBBoxPCustom ERROR invalid viewport!\n");
		return 0;
	}
	glEnable(GL_DEPTH_TEST);
	vbo_bind(&bboxrenderer_bbox_custom_vao);
	CHECKGLERROR
	shader_t *s = shader_returnById(bboxrenderer_lineshader_id);
	glUseProgram(s->programid);
//	matrix4x4_t tmath;
//	Matrix4x4_Concat(&tmath, &v->viewproj, mat);
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&v->viewproj, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	CHECKGLERROR
	glBindBuffer(GL_ARRAY_BUFFER, bboxrenderer_bbox_custom_vao.vertsid[0]);
//	glBufferData(GL_ARRAY_BUFFER, numverts * bboxrenderer_bbox_custom_vao.datawidth[0] * sizeof(GLfloat), posdata, GL_DYNAMIC_DRAW);
	//cheap hack lel
	glBufferSubData(GL_ARRAY_BUFFER, 0, 24 *sizeof(GLfloat), bboxp);
	glBufferSubData(GL_ARRAY_BUFFER, 24 *sizeof(GLfloat), 24 *sizeof(GLfloat), bboxp);
	CHECKGLERROR
	glDrawElements(GL_LINES, bboxrenderer_bbox_custom_vao.numfaces*2, GL_UNSIGNED_INT, 0);
	CHECKGLERROR
	return TRUE;
}


//unused as of now, but may be useful for things already doing the concats n shit
int bboxrenderer_renderBBoxTmat(float tmat[16]){
	glEnable(GL_DEPTH_TEST);
	vbo_bind(&bboxrenderer_bbox_vao);
	CHECKGLERROR
	shader_t *s = shader_returnById(bboxrenderer_lineshader_id);
	glUseProgram(s->programid);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	CHECKGLERROR
	glDrawElements(GL_LINES, bboxrenderer_bbox_vao.numfaces*2, GL_UNSIGNED_INT, 0);
	CHECKGLERROR
	return TRUE;
}

int bboxrenderer_shutdown(void){
	//TODO
	//todo cleanly free up shader and model
	return TRUE;
}

