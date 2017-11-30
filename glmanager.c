#include <epoxy/gl.h>
#include <epoxy/glx.h> //opengl includes
#include <math.h>

#include "globaldefs.h"

#include "glmanager.h"




#include "vbomanager.h"		//for init
#include "modelmanager.h"	//for init, register, load

#include "shadermanager.h"	//for init, register, load


#include "matrixlib.h"			//temp
	//TEMP
#include "viewportmanager.h"		//uh... do i need this here?
#include "mathlib.h"
//extern int viewport_init(void);
//extern int viewport_shutdown(void);

#include "tracegrid.h" // for tracegrid duh

#include "planebox.h"


extern int glfw_context1(void);
extern int glfw_context2(void);

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
//	planebox_shutdown();
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

planebox_t tmp = {0};
viewport_t tmpvst = {0};

//true is everything initialized correctly
int gl_init(void){

	glGetIntegerv(GL_MAX_SAMPLES,			&msaa_maxSamples);
	glGetIntegerv(GL_MAX_INTEGER_SAMPLES,		&msaa_maxIntSamples);
	glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES,	&msaa_maxColorSamples);
	glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES,	&msaa_maxDepthSamples);

	printf("MSAA Samples: %i, int %i, color %i, depth %i\n", msaa_maxSamples, msaa_maxIntSamples, msaa_maxColorSamples, msaa_maxDepthSamples);

	vbo_init();
	model_init();
	shader_init();
	tracegrid_init();
	viewport_init();
	planebox_init();



//second context stuff
	glfw_context2();
	CHECKGLERROR
	planebox_initOtherContext();
	tmp.type = 1;
	tmp.name = strdup("tmp.planebox");
	planebox_load(&tmp);
	//everything but the vao should work in context 1
	CHECKGLERROR
	glfw_context1();



	tmpvst.type = 1;
	tmpvst.fov = 30.0;
	tmpvst.aspect = 4.0/3.0;
	tmpvst.far = 10.0;
	tmpvst.near = 0.1;
	tmpvst.changed = 3;
	tmpvst.pos[2] = 5.0;
	viewport_recalc(&tmpvst);

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
int gl_renderFrame(void){ //temp

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
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

	glBindVertexArray(m->vbo.vaoid);
	CHECKGLERROR
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&v.viewproj, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	CHECKGLERROR
	glDrawElements(GL_TRIANGLES, m->vbo.numverts * 3, GL_UNSIGNED_INT, 0);
	CHECKGLERROR
*/
	return 1;
}

int gl_renderDebug(void){
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	int mid = model_register("models/bunny.iqm");
	model_t *m = model_returnById(mid);
	model_load(m);
//	print("loaded model %s with %i verts and %i tris");
//	if(m->type) printf("error\n");
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

//	printf("bunny vao id %i\n", m->vbo.vaoid);
	glBindVertexArray(m->vbo.vaoid);
	CHECKGLERROR
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&v.viewproj, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	CHECKGLERROR
//	printf("%i tris\n", m->vbo.numfaces);
//	glDrawElements(GL_TRIANGLES, m->vbo.numfaces * 3, GL_UNSIGNED_INT, 0);
	CHECKGLERROR

	planebox_renderDebug(&tmp, &v);
	planebox_renderDebugLines(&tmp, &v);
	planebox_renderViewportDebugLines(&tmpvst, &v);
	return 1;
}
