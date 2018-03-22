#include <epoxy/gl.h>
#include <math.h>

#include "globaldefs.h"

#include "glmanager.h"


#include "contextmanager.h"

#include "vbomanager.h"
#include "ubomanager.h"
#include "renderqueue.h"

#include "modelmanager.h"

#include "shadermanager.h"

#include "matrixlib.h"

#include "mathlib.h"

#include "viewportmanager.h"


#include "firstbouncerenderer.h"



int firstbounce_deferredmodelshader_id = 0;

int firstbouncerenderer_init(void){
	firstbounce_deferredmodelshader_id = shader_register("shaders/deferredmodel.program");
	shader_t *s = shader_returnById(firstbounce_deferredmodelshader_id);
	shader_load(s);
	return TRUE;
}


int firstbouncerenderer_shutdown(void){
	//todo
	return FALSE;
}

typedef struct deferredModelUBOStruct_s {
	GLfloat mvp[16];
	GLfloat m[16];
	GLfloat v[16];
} deferredModelUBOStruct_t;
void firstbounce_setupModelCallback(renderqueue_t *q, renderlistitem_t * ilist, unsigned int count){
//	if(count >1){
	//assume no instancing for now
//		unsigned int i = 0;
//		while(i < count){
//			deferredModelCallbackData_t *d = ilist[i].data;
//			unsigned int counter = 0;
//			//push muh data
//			
//		}
//	}
	//assume no UBO yet
	deferredModelCallbackData_t *d = ilist->data;
	deferredModelUBOStruct_t ubodata;
	memcpy(ubodata.mvp, d->mvp, 16 * sizeof(GLfloat));
	memcpy(ubodata.m,   d->m,   16 * sizeof(GLfloat));
	memcpy(ubodata.v,   d->v,   16 * sizeof(GLfloat));
	int t = ubo_pushData(&q->ubo, sizeof(ubodata), &ubodata);
	d->uboDataOffset = t;
}


int firstbounce_renderModel(viewport_t *v, int mid, matrix4x4_t *mat){
	glEnable(GL_DEPTH_TEST);
	//bind model

	model_t *m = model_returnById(mid);
	if(!m){
		printf("FIRSTBOUNCE/renderModel: ERROR invalid model id! %i\n", mid);
		return 0;
	}
	model_bind(m);

	shader_t *s = shader_returnById(firstbounce_deferredmodelshader_id);
	glUseProgram(s->programid);
//	matrix4x4_t tmath;
//	Matrix4x4_Concat(&tmath, &v->viewproj, mat);
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&v->viewproj, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	Matrix4x4_ToArrayFloatGL(mat, tmat);
	glUniformMatrix4fv(s->uniloc[1], 1, GL_FALSE, tmat);
	Matrix4x4_ToArrayFloatGL(&v->view, tmat);
	glUniformMatrix4fv(s->uniloc[2], 1, GL_FALSE, tmat);

	CHECKGLERROR

	//draw model
	glDrawElements(GL_TRIANGLES, m->vbo.numfaces *3, GL_UNSIGNED_INT, 0);


	return m->vbo.numfaces;
}
