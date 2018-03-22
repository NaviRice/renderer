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

void firstbounce_drawModelCallback(renderqueue_t *q, renderlistitem_t *ilist, unsigned int count){
	glEnable(GL_DEPTH_TEST);
	deferredModelCallbackData_t *d = ilist->data;
	model_t *m = model_returnById(d->modelid);
	model_bind(m);
	unsigned int mysize = count * sizeof(deferredModelUBOStruct_t);
	//make sure to bind UBO correctly
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, q->ubo.id, d->uboDataOffset, mysize);
//	glBindBufferBase(GL_UNIFORM_BUFFER, i, id);
	shader_t *s = shader_returnById(firstbounce_deferredmodelshader_id);
	glUseProgram(s->programid);


//hacky shit
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, d->u.vp);
	glUniformMatrix4fv(s->uniloc[1], 1, GL_FALSE, d->u.m);
	glUniformMatrix4fv(s->uniloc[2], 1, GL_FALSE, d->u.v);

	glDrawElementsInstanced(GL_TRIANGLES, m->vbo.numfaces * 3, GL_UNSIGNED_INT, 0, count);
}
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
	//todo add functions in UBO data push to help facilitate this
	deferredModelCallbackData_t *d = ilist->data;
	int t = ubo_pushData(&q->ubo, sizeof(deferredModelUBOStruct_t), &d->u);
	d->uboDataOffset = t;
}
void firstbounce_addModelToRenderQueue(renderqueue_t *q, viewport_t *v, int mid, matrix4x4_t *mat){
	renderlistitem_t r;
	deferredModelCallbackData_t d;
	d.modelid = mid;


	Matrix4x4_ToArrayFloatGL(&v->viewproj, d.u.vp);
	Matrix4x4_ToArrayFloatGL(mat, d.u.m);
	Matrix4x4_ToArrayFloatGL(&v->view, d.u.v);

//todo do sort
	r.sort[0] = 0;
	r.sort[1] = 0;
	r.sort[2] = 0;
	r.sort[3] = 0;
	r.sort[4] = 0;
	r.sort[5] = 0;
	r.sort[6] = 0;
	r.sort[7] = 0;
	r.sort[8] = 0;
	r.sort[9] = 0;


	r.datasize = sizeof(d);
	r.data = &d;
	r.flags = 2|4;
	r.setup = firstbounce_setupModelCallback;
	r.draw = firstbounce_drawModelCallback;

	//add it u fuck
	renderqueue_addtoqueue(q, r);
}

renderqueue_t hackyshit = {0};
int firstbounce_renderModel(viewport_t *v, int mid, matrix4x4_t *mat){
	//hacky test shit
#if 1
	firstbounce_addModelToRenderQueue(&hackyshit, v, mid, mat);
	renderqueue_sort(&hackyshit);
	renderqueue_setup(&hackyshit);
	renderqueue_flushbuffers(&hackyshit);
	renderqueue_draw(&hackyshit);


#else
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
#endif
}
