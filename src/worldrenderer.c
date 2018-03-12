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
#include "entitymanager.h"

#include "mathlib.h"
#include "viewportmanager.h"

#include "worldrenderer.h"
#include "bboxrenderer.h"

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

int worldrenderer_addModelToQueue(renderqueue_t *q, viewport_t *v, int mid, matrix4x4_t *mat, vec4_t color){
	//TODO
	return FALSE;
}



int worldrenderer_renderModel(viewport_t *v, int mid, matrix4x4_t *mat, vec4_t color){
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
	glUniform4fv(s->uniloc[1], 1, color);
	glDrawElements(GL_TRIANGLES, m->vbo.numfaces * 3, GL_UNSIGNED_INT, 0);
	return m->vbo.numfaces;
}




int worldrenderer_recalcFakeVP(viewport_t *v){ //todo change this to also do shrinking based on windshield bboxp

//	v->pos[0] = 0.0;
//	v->pos[1] = 0.0;
//	v->pos[2] = 4.0;
	int i;
	int counter = 0;
	float maxpitch = -10000.0;
	float minpitch = 10000.0;
	float maxyaw = -10000.0;
	float minyaw = 10000.0;
	for(i=0; i <= entity_arraylasttaken; i++){
		entity_t *e = entity_list + i;
		//if entities can be rendered as anything other than models, i have to change this
		if(!e->myid || !e->modelid)continue;
		int z;
		for(z = 0; z < 8; z++){
			vec_t *v1 = &e->bboxp[z*3];
//			vec_t *v1 = &e->finalpos;
			vec3_t v2;
			vec3subvec(v2, v->pos, v1);
//			vec3norm2(v2, v2);


		//todo i might be able to pull some of this outside the loop
			float pitch = atan2(v2[1], sqrt(v2[0]*v2[0]+v2[2]*v2[2]));
			float yaw = atan2(-v2[0], v2[2]);
//			float pitch = asin(v2[1]);
//			printf("v1 is %f %f %f, v->pos is %f %f %f\n", v1[0], v1[1], v1[2], v->pos[0], v->pos[1], v->pos[2]);
//			printf("v2 is %f %f %f, pitch is %f\n", v2[0], v2[1], v2[2], pitch);

			if(!counter) maxpitch = minpitch = pitch;
			else if(pitch > maxpitch) maxpitch = pitch;
			else if(pitch < minpitch) minpitch = pitch;


			//todo make the legit 2 pass method
			if(!counter) maxyaw = minyaw = yaw;
			else if(yaw > maxyaw) maxyaw = yaw;
			else if(yaw < minyaw) minyaw = yaw;

			counter++;
		}
	}
	if(!counter) return FALSE;
	maxpitch *= 180.0/M_PI;
	minpitch *= 180.0/M_PI;
	maxyaw *= 180.0/M_PI;
	minyaw *= 180.0/M_PI;

	//im probably going to recalc fov, aspect, near, far, angle

	//calculate pitch angles


/*
	if(maxpitch < 0.0) maxpitch+= 360.0;
	else if(maxpitch >= 360.0) maxpitch -= 360.0;
	if(minpitch < 0.0) minpitch+= 360.0;
	else if(minpitch >= 360.0) minpitch -= 360.0;
*/
	v->angle[0] = (maxpitch+minpitch) * 0.5;

//	v->angle[0] = minpitch;

	//calculate yaw angles
//	float maxyaw = -atan2(maxs[0], -maxs[2]) * 180.0/M_PI;
//	float minyaw = -atan2(mins[0], -mins[2]) * 180.0/M_PI;



//	printf("maxpitch minpitch %f %f ", maxpitch, minpitch);
//	printf("maxyaw minyaw %f %f\n", maxyaw, minyaw);



	v->angle[1] = (maxyaw+minyaw) * 0.5;
//	v->angle[1] = maxyaw;



	float t1 = maxpitch - minpitch;
	float t2 = fabs(maxpitch - (minpitch + 360.0));

//	v->fov = fabs(maxpitch-minpitch);
	v->fov = t1 < t2 ? t1 : t2;
	if(v->fov > 130.0) v->fov = 130.0;


	float y1 = maxyaw - minyaw;
	float y2 = fabs(maxyaw - (minyaw + 360.0));

//	float hfov = fabs(maxyaw-minyaw);
	float hfov = y1 < y2 ? y1 : y2;
	if(hfov > 130.0) hfov = 130.0;
	v->aspect = hfov/v->fov;

	v->changed |= 3; // 1 and 2
//	v->aspect = 1.0;

//	printf("maxpitch is %f\n", maxpitch);

	return TRUE;
}


int worldrenderer_addEntitiesToQueue(renderqueue_t *q, viewport_t *v){
	//todo
	int i;
	int counter = 0;
	for(i = 0; i <= entity_arraylasttaken; i++){
		entity_t *e = entity_list+i;
		if(!e->myid || !e->modelid) continue;
		worldrenderer_addModelToQueue(q, v, e->modelid, &e->mat, e->color);
		counter++;
	}
	return counter;
}

int worldrenderer_renderEntities(viewport_t *v){
	int i;
	int counter = 0;
	for(i=0 ; i <= entity_arraylasttaken; i++){
		entity_t *e = &entity_list[i];
		if(!e->myid || !e->modelid)continue;
//		worldrenderer_renderModel(v, e->modelid, &e->final, e->color);
//todo adjust ents so they have a final
		worldrenderer_renderModel(v, e->modelid, &e->mat, e->color);
		counter++;
	}
	return counter;
}


int worldrenderer_renderEntitiesBBoxes(viewport_t *v){
	int i;
	int counter = 0;
	for(i=0 ; i <= entity_arraylasttaken; i++){
		entity_t *e = &entity_list[i];
		if(!e->myid)continue;
		//todo adjust ents so they have a final
//		bboxrenderer_renderBBoxMat(v, &e->mat);
//vec_t fuckyou[24] = {
//e->bbox[1], e->bbox[3], e->bbox[5],	e->bbox[0], e->bbox[3], e->bbox[5],	e->bbox[0], e->bbox[2], e->bbox[5],	e->bbox[1], e->bbox[2], e->bbox[5],
//e->bbox[1], e->bbox[3], e->bbox[4],	e->bbox[0], e->bbox[3], e->bbox[4],	e->bbox[0], e->bbox[2], e->bbox[4],	e->bbox[1], e->bbox[2], e->bbox[4]};

		bboxrenderer_renderBBox(v, e->bbox);
		bboxrenderer_renderBBoxPCustom(v, e->bboxp);
//		bboxrenderer_renderBBoxPCustom(v, fuckyou);
//		bboxrenderer_renderBBoxMat(v, &e->mat);
		counter++;
	}
	return counter;
}


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

	//dumb hack


	//basically just flip across the X axis, but flip the image first so its not mirrored
	Matrix4x4_CreateScale3(&tmath, -1.0, 1.0, 1.0);
	Matrix4x4_ConcatTranslate(&tmath, scale * box-1.0, scale * view-1.0 , 0.0);
	Matrix4x4_ConcatScale3(&tmath, -1.0 *scale * box, scale * view, 1.0);

	Matrix4x4_ToArrayFloatGL(&tmath, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	fsquad_render();
	return TRUE;
}
