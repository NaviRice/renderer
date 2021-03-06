
#include <epoxy/gl.h>
#include <math.h>

#include "globaldefs.h"

#include "glmanager.h"




#include "contextmanager.h"
#include "vbomanager.h"		//for init
#include "ubomanager.h"		//for init
#include "renderqueue.h"
#include "modelmanager.h"	//for init, register, load
#include "text.h"		//for init

#include "shadermanager.h"	//for init, register, load


#include "matrixlib.h"			//temp
	//TEMP
#include "viewportmanager.h"		//uh... do i need this here?
#include "mathlib.h"
//extern int viewport_init(void);
//extern int viewport_shutdown(void);


#include "planebox.h"
#include "tracegrid.h" // for tracegrid duh
#include "firstbouncerenderer.h"
#include "worldrenderer.h"
#include "bboxrenderer.h"
#include "fsquad.h"


#include "contextmanager.h"


#include "headclient.h"




//todo move into glinit and then just keep track of the lowest?
int msaa_maxSamples=0, msaa_maxIntSamples=0, msaa_maxColorSamples=0, msaa_maxDepthSamples=0;



vbo_t planetris;


int gl_shutdown(void){
	printf("renderer shutting down\n");

	shader_shutdown();
	model_shutdown();
	vbo_shutdown();
	viewport_shutdown();
	tracegrid_shutdown();
	planebox_shutdown();
	//todo add the renderer shutdowns here
	return TRUE; //successful shutdown
}

void gl_printError(int errornumber, const char *filename, int linenumber){
	switch(errornumber){
#ifdef GL_INVALID_ENUM
	case GL_INVALID_ENUM:
		printf("GL_INVALID_ENUM at %s:%i\n", filename, linenumber); break;
#endif
#ifdef GL_INVALID_VALUE
	case GL_INVALID_VALUE:
		printf("GL_INVALID_VALUE at %s:%i\n", filename, linenumber); break;
#endif
#ifdef GL_INVALID_OPERATION
	case GL_INVALID_OPERATION:
		printf("GL_INVALID_OPERATION at %s:%i\n", filename, linenumber); break;
#endif
#ifdef GL_STACK_OVERFLOW
	case GL_STACK_OVERFLOW:
		printf("GL_STACK_OVERFLOW at %s:%i\n", filename, linenumber); break;
#endif
#ifdef GL_STACK_UNDERFLOW
	case GL_STACK_UNDERFLOW:
		printf("GL_STACK_UNDERFLOW at %s:%i\n", filename, linenumber); break;
#endif
#ifdef GL_OUT_OF_MEMORY
	case GL_OUT_OF_MEMORY:
		printf("GL_OUT_OF_MEMORY at %s:%i\n", filename, linenumber); break;
#endif
#ifdef GL_TABLE_TOO_LARGE
	case GL_TABLE_TOO_LARGE:
		printf("GL_TABLE_TOO_LARGE at %s:%i\n", filename, linenumber); break;
#endif
#ifdef GL_INVALID_FRAMEBUFFER_OPERATION
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		printf("GL_INVALID_FRAMEBUFFER_OPERATION at %s:%i\n", filename, linenumber); break;
#endif
	default:
		printf("GL UNKNOWN (%i) at %s:%i\n", errornumber, filename, linenumber); break;
	}
}

//planebox_t tmp = {0};
planebox_t tmpscreen = {0};
viewport_t tmpvst = {0};
viewport_t debugvp = {0};
int windshieldid = {0};
matrix4x4_t windshieldmat;


//true is everything initialized correctly
int gl_init(void){
	glDepthFunc(GL_LEQUAL);

	glGetIntegerv(GL_MAX_SAMPLES,			&msaa_maxSamples);
	glGetIntegerv(GL_MAX_INTEGER_SAMPLES,		&msaa_maxIntSamples);
	glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES,	&msaa_maxColorSamples);
	glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES,	&msaa_maxDepthSamples);

	printf("MSAA Samples: %i, int %i, color %i, depth %i\n", msaa_maxSamples, msaa_maxIntSamples, msaa_maxColorSamples, msaa_maxDepthSamples);

//temp

	vbo_init();
	ubo_init();
	model_init();
	shader_init();
	viewport_init();
	firstbouncerenderer_init();
	worldrenderer_init();
	planebox_init();
	bboxrenderer_init();
	fsquad_init();
	tracegrid_init();
	text_init();



//	tmp.type = 1;
//	tmp.name = strdup("planeboxes/tmp.planebox");
//	planebox_load(&tmp);

	tmpscreen.type = 1;
	tmpscreen.name = strdup("planeboxes/screen.planebox");
	planebox_load(&tmpscreen);

	debugvp.type = 1;
	debugvp.fov = 90.0;
	debugvp.aspect = 1.0;
	debugvp.far = 1000.0;
	debugvp.near = 0.1;
	debugvp.changed = 3;
	viewport_recalc(&debugvp);

	tmpvst.type = 1;
	tmpvst.fov = 30.0;
	tmpvst.aspect = 4.0/3.0;
	tmpvst.far = 20.0;
	tmpvst.near = 0.1;
	tmpvst.changed = 3;
	tmpvst.pos[2] = 5.0;
	viewport_recalc(&tmpvst);

	windshieldid = model_register("models/testwindshield.iqm");
	model_load(model_returnById(windshieldid));
	Matrix4x4_CreateFromQuakeEntity(&windshieldmat, 0, 0, 0.0, 0, 0, 0, 1.0);


	tracegrid_resize(100,100);


	//mid = model_register("models/teapot.iqm");
//	printf("registered model id %i\n", mid);
//	model_load(model_returnById(mid));

//	int mid = model_register("test.iqm");
//	printf("registered model id %i\n", mid);
//	model_load(model_returnById(mid));
//	CHECKGLERROR

//	int sid = shader_register("shaders/test.program");
//	printf("registered shader id %i\n", sid);
//	shader_load(shader_returnById(sid));
//	CHECKGLERROR

	return TRUE;
}

int cnt = 0;
//ALL OF THIS TEMP
int gl_renderFrame(double time){ //temp
	mycontext_t *c = mycontexts+context_current;
	int width = c->width;
	int height = c->height;
	if(c->changed){
		c->changed = FALSE;
//		debugvp.aspect = (double)width/(double)height;
//		debugvp.changed |=2;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0,0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	tracegrid_renderGrid(&tmpvst, &debugvp, &tmpscreen);
	tracegrid_renderOutput(&tmpvst, &debugvp, &tmpscreen);


//	tracegrid_renderDebugFirstbounce(&tmpvst, &debugvp);
//	tracegrid_renderDebugGridMini(&tmpvst, &debugvp);
/*	int mid = model_register("models/coil.iqm");
	model_t *m = model_returnById(mid);
	model_load(m);
	int sid = shader_register("shaders/test.program");
	shader_t *s = shader_returnById(sid);
	shader_load(s);
	viewport_t v = {0};
	v.type = 1;
	v.fov = 90.0;
	v.aspect = 4.0/3.0;
	v.far = 1000.0;
	v.near = 0.1;
	v.angle[1] = cnt;
	v.angle[0] = 30.0;
	v.pos[0] = sin((v.angle[1]/180.0) * -M_PI) * 10.0;
	v.pos[2] = cos((v.angle[1]/180.0) * -M_PI) * 10.0;
	v.pos[1] = 3.0;

	cnt++;
	v.changed = 3;
	viewport_recalc(&v);

	glUseProgram(s->programid);
	CHECKGLERROR

	vbo_bind(&m->vbo);
	CHECKGLERROR
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&v.viewproj, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	CHECKGLERROR
	glDrawElements(GL_TRIANGLES, m->vbo.numfaces * 3, GL_UNSIGNED_INT, 0);
	CHECKGLERROR
*/
	return 1;
}

renderqueue_t debugopaque = {0};
renderqueue_t worldopaque = {0};

int gl_renderWorld(double time){
	worldrenderer_bindEASTEREGG();
	GLenum renderbuffs[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, renderbuffs);//todo move
	glClearColor(0.0, 0.0, 0.0, 0.0);
//	glClearColor(0.0, 0.0, 0.1, 0.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	//todo move this to a different function?
	//todo move this to a different thread
	worldrenderer_addEntitiesToQueue(&worldopaque, &debugvp);

	worldrenderer_renderEntities(&tmpvst);
//	worldrenderer_renderEntitiesBBoxes(&tmpvst);

	return TRUE;
}
extern int worldrenderer_modelshader_id;

int gl_renderWorldDebug(double time){
	//todo

	//todo move this to a different function?
	//todo move this to a different thread
	worldrenderer_addEntitiesToQueue(&debugopaque, &debugvp);

	worldrenderer_renderEntities(&debugvp);
//	worldrenderer_renderEntitiesBBoxes(&debugvp);
	return TRUE;
}


#ifndef USENEWRENDERER
int gl_renderFirstbounce(double time){
	tracegrid_bindBounce();
	GLenum renderbuffs[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, renderbuffs);//todo move
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	firstbounce_renderModel(&tmpvst, windshieldid, &windshieldmat);
//	planebox_renderFirstbounce(&tmp, &tmpvst);
	return TRUE;
}
#endif


extern viewport_t headclient_kinectvp;
extern vec3_t headclient_headpos;
#ifndef USENEWRENDERER
int gl_renderDebug(double time){
	mycontext_t *c = mycontexts+context_current;
	int width = c->width;
	int height = c->height;
	if(c->changed){
		c->changed = FALSE;
		debugvp.aspect = (double)width/(double)height;
		debugvp.changed |=2;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0,0, width, height);
//	glClearColor(0.1, 0.1, 0.1, 0.0);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	debugvp.angle[1] = time*10.0;
	debugvp.angle[0] = 30.0;
	debugvp.pos[0] = sin((debugvp.angle[1]/180.0) * -M_PI) * 10.0;
	debugvp.pos[2] = cos((debugvp.angle[1]/180.0) * -M_PI) * 10.0;
	debugvp.pos[1] = 3.0;
	debugvp.changed |= 1;


//	debugvp.angle[0] = 90.0;
//	debugvp.angle[1] = 0.0;
//	debugvp.pos[0] = debugvp.pos[2] = 0;
//	debugvp.pos[1] = 10.0;
	viewport_recalc(&debugvp);



//	tmpvst.angle[1] = sin(time * 1.0) * 10;
//	tmpvst.angle[0] = sin(time * 3.1415926868) * 10;
//	tmpvst.pos[0] = sin(time * 3.111111111);
//	tmpvst.pos[1] = sin(time * 2.111111111);
//	tmpvst.pos[2] = 5.0 + sin(time * 1.11111111);

	tmpvst.pos[0] = headclient_headpos[0];
	tmpvst.pos[1] = headclient_headpos[1];
	tmpvst.pos[2] = headclient_headpos[2];
	tmpvst.changed |= 1;
	//todo move
	worldrenderer_recalcFakeVP(&tmpvst);
	viewport_recalc(&tmpvst);

	cnt++;

//	planebox_renderDebug(&tmp, &debugvp);

//	bboxrenderer_renderBBoxMat(&debugvp, &tmp.model);

	//planebox_renderDebugLines(&tmp, &debugvp);
	bboxrenderer_renderBBoxMat(&debugvp, &tmpscreen.model);
//	planebox_renderDebugLines(&tmpscreen, &debugvp);
	bboxrenderer_renderBBoxMat(&debugvp, &tmpvst.viewprojinv);
//	planebox_renderViewportDebugLines(&tmpvst, &debugvp);
	bboxrenderer_renderBBoxMat(&debugvp, &headclient_kinectvp.viewprojinv);
//	planebox_renderViewportDebugLines(&headclient_kinectvp, &debugvp);

	vec4_t windshieldcolor = {0.4, 0.5, 0.6, 0.3};
	gl_renderWorldDebug(time);

//	tracegrid_renderDebugOutput(&tmpvst, &debugvp, &tmpscreen);
	tracegrid_renderDebugGrid(&tmpvst, &debugvp, &tmpscreen);

	worldrenderer_renderModelTransparent(&debugvp, windshieldid, &windshieldmat, windshieldcolor);

//	tracegrid_renderDebugFirstbounce(&tmpvst, &debugvp);
//	tracegrid_renderDebugGridMini(&tmpvst, &debugvp);

	worldrenderer_renderDebugFramebufferMini(&tmpvst, &debugvp);



//	float phi = sin(time / 5.0) * 10;
//	phi*=phi;
//	tracegrid_resize((int)phi+2, (int)phi+2);
	return TRUE;
}

#endif

//not used anymore
/*
int gl_resizeViewports(int width, int height){
	return TRUE;
}

int gl_resizeDebugViewports(int width, int height){
	//todo framebuffer class that keeps track of height
	debugwidth = width;
	debugheight = height;
	debugvp.aspect = (double)debugwidth/(double)debugheight;
	debugvp.changed |=2;
	return TRUE;
}
*/




#ifdef USENEWRENDERER
int gl_setupFirstbounceForRender(double time){
	//really just have to render the windshield
	return TRUE;
}
int gl_setupWorldForRender(double time){
	return TRUE;
}
int gl_setupDebugForRender(double time){
	return TRUE;
}




int gl_renderFirstbounce(double time){
	return TRUE;
}
int gl_renderScreen(double time){
	return TRUE;
}
int gl_renderDebug(double time){
	return TRUE;
}
#endif
