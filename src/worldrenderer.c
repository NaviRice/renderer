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

#include "worldrenderer.h"

#include "fsquad.h"



//todo move this shit to a framebuffer manager
GLuint worldrenderer_fbo[NUMCONTEXTS] = {0};
GLuint worldrenderer_fbo_postex = 0;
GLuint worldrenderer_fbo_renderbuffer = 0;

int worldrenderer_fbo_height = 0;
int worldrenderer_fbo_width = 0;

int worldrenderer_modelshader_id = 0;




int worldrenderer_initFramebufferonly(void){
	glGenFramebuffers(1, &worldrenderer_fbo[context_current]);
	glBindFramebuffer(GL_FRAMEBUFFER, worldrenderer_fbo[context_current]);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("werefucked1\n");
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, worldrenderer_fbo_postex, 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("werefucked2\n");
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, worldrenderer_fbo_renderbuffer);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("werefucked4\n");
	return TRUE;
}


int worldrenderer_bindEASTEREGG(void){
	if(!worldrenderer_fbo[context_current]){
		worldrenderer_initFramebufferonly();;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, worldrenderer_fbo[context_current]);
	glViewport(0,0, worldrenderer_fbo_width, worldrenderer_fbo_height);
	CHECKGLERROR
	return TRUE;
}


//todo move to a framebuffer manager
int worldrenderer_initFramebuffer(int width, int height){
	glGenTextures(1, &worldrenderer_fbo_postex);
	glBindTexture(GL_TEXTURE_2D, worldrenderer_fbo_postex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	CHECKGLERROR

	glGenRenderbuffers(1, &worldrenderer_fbo_renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, worldrenderer_fbo_renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

	worldrenderer_fbo_width = width;
	worldrenderer_fbo_height = height;

//	worldrenderer_initFramebufferonly();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return TRUE;
}


int worldrenderer_init(void){
	worldrenderer_initFramebuffer(1024, 1024);
	worldrenderer_modelshader_id = shader_register("shaders/model.program");
	shader_t *s = shader_returnById(worldrenderer_modelshader_id);
	shader_load(s);
	//TODO
	return TRUE;
}

int worldrenderer_shutdown(void){
	//TODO
	//todo framebuffer stuff
	return TRUE;
}


int worldrenderer_renderModel(viewport_t *v, int mid, matrix4x4_t *mat){
//	Matrix4x4_Print(mat);
	model_t *m = model_returnById(mid);
	if(!m){
		printf("WORLDRENDERER/renderModel: ERROR invalid model id! %i\n", mid);
		return 0;
	}
	model_bind(m);
	shader_t *s = shader_returnById(worldrenderer_modelshader_id);
	glUseProgram(s->programid);
	matrix4x4_t tmath;
	float tmat[16];
	Matrix4x4_Concat(&tmath, &v->viewproj, mat);
	Matrix4x4_ToArrayFloatGL(&tmath, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	glDrawElements(GL_TRIANGLES, m->vbo.numfaces * 3, GL_UNSIGNED_INT, 0);
}




int worldrenderer_recalcFakeVP(viewport_t *v){ //todo change this to also do shrinking based on windshield bboxp
	vec3_t mins = {10000.0, 10000.0, 10000.0};
	vec3_t maxs = {-10000.0, -10000.0, -10000.0};
	int i;
	int counter;
	for(i=counter=0; i <= entity_arraylasttaken; i++){
		entity_t *e = &entity_list[i];
		if(!e->myid)continue;
		counter++;
		int z;
		for(z = 0; z < 6; z++){
			vec_t *v1 = &e->bboxp[z*3];
			vec3_t v2;
			vec3_t v3;
			vec3subvec(v2, v1, v->pos);
			vec3norm2(v3, v2);
			if(v3[0] < mins[0]) mins[0] = v3[0];
			else if(v3[0] > maxs[0]) maxs[0] = v3[0];
			if(v3[1] < mins[1]) mins[1] = v3[1];
			else if(v3[1] > maxs[1]) maxs[1] = v3[1];
			if(v3[2] < mins[2]) mins[2] = v3[2];
			else if(v3[2] > maxs[2]) maxs[2] = v3[2];
		}

	}
	if(!counter) return FALSE;

	//im probably going to recalc fov, aspect, near, far, angle
	//pos stays the same
	return TRUE;
}

int worldrenderer_renderEntities(viewport_t *v){
	int i;
	int counter = 0;
	for(i=0 ; i <= entity_arraylasttaken; i++){
		entity_t *e = &entity_list[i];
		if(!e->myid || !e->modelid)continue;
//		worldrenderer_renderModel(v, e->modelid, &e->final);
		worldrenderer_renderModel(v, e->modelid, &e->mat);
		counter++;
	}
	return counter;
}
//todo
/*
int worldrenderer_renderEntitiesBboxes(viewport_t *v){
	int i;
	int counter = 0;
	for(i=0 ; i <= entity_arraylasttaken; i++){
		entity_t *e = &entity_list[i];
		if(!e->myid || !e->modelid)continue;
//		worldrenderer_renderModel(v, e->modelid, &e->final);
		worldrenderer_renderModel(v, e->modelid, &e->mat);
		counter++;
	}
	return counter;
}
*/


extern int tracegrid_debugfirstbounceshader_id;
int worldrenderer_renderDebugFramebufferMini(viewport_t * caster, viewport_t *v){
	if(!v || !v->type){
		 printf("WORLDRENDERER/renderDebugFramebufferMini: ERROR invalid viewport!\n");
		return 0;
	}
	if(!caster || !caster->type){
		 printf("WORLDRENDERER/renderDebugFramebufferMini: ERROR invalid caster viewport!\n");
		return 0;
	}
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, worldrenderer_fbo_postex);
	glActiveTexture(GL_TEXTURE0);
	shader_t *s = shader_returnById(tracegrid_debugfirstbounceshader_id);
	glUseProgram(s->programid);

	glDisable(GL_DEPTH_TEST);
	float tmat[16];
	matrix4x4_t tmath;
	float box = caster->aspect;
	float view = v->aspect;
	float scale = box > view ? 0.33/box : 0.33/view;
	Matrix4x4_CreateTranslate(&tmath, scale * (box + 1.0), scale * view-1.0 , 0.0);
	Matrix4x4_ConcatScale3(&tmath, scale * box, scale * view, 1.0);
	Matrix4x4_ToArrayFloatGL(&tmath, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	fsquad_render();
	return TRUE;
}
