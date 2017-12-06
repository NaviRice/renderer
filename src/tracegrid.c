#include <epoxy/gl.h>
#include "glmanager.h" // for CHECKGLERROR
#include "globaldefs.h"

#include "matrixlib.h"
#include "viewportmanager.h"
#include "tracegrid.h"

#include "shadermanager.h"
#include "vbomanager.h"


#include "fsquad.h" // for render of debug

vbo_t tracegrid_vao = {0};
int tgwidth = 0;
int tgheight = 0;
int tgtriangles = 0;

int tracegrid_debuggridshader_id = 0;
int tracegrid_debuggridminishader_id = 0;
int tracegrid_firstbounceshader_id = 0;
int tracegrid_debugfirstbounceshader_id = 0;




//tmp
//todo multiple contexts and fbo sharing issues... probably how im gonna do it is just render in the main context
GLuint tracegrid_fbo = 0;
GLuint tracegrid_fbo_postex = 0; //maybe change to depth, idk
GLuint tracegrid_fbo_normtex = 0;
GLuint tracegrid_fbo_renderbuffer = 0;

int tracegrid_fbo_width = 0;
int tracegrid_fbo_height = 0;

//todo really move this to a framebuffermanager
int tracegrid_bindBounce(void){
	glBindFramebuffer(GL_FRAMEBUFFER, tracegrid_fbo);
	glViewport(0,0, tracegrid_fbo_width, tracegrid_fbo_height);
	shader_t *s = shader_returnById(tracegrid_firstbounceshader_id);
	glUseProgram(s->programid);
	CHECKGLERROR
	//todo
	return TRUE;
}


//todo move to a framebuffer manager
int tracegrid_initFramebuffer(int width, int height){
	glGenFramebuffers(1, &tracegrid_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, tracegrid_fbo);
	glGenTextures(1, &tracegrid_fbo_postex);
	glGenTextures(1, &tracegrid_fbo_normtex);
	glBindTexture(GL_TEXTURE_2D, tracegrid_fbo_postex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tracegrid_fbo_postex, 0);
	CHECKGLERROR
	glBindTexture(GL_TEXTURE_2D, tracegrid_fbo_normtex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tracegrid_fbo_normtex, 0);


	glGenRenderbuffers(1, &tracegrid_fbo_renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, tracegrid_fbo_renderbuffer);
	//look into depth32 and depth32f
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, tracegrid_fbo_renderbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	tracegrid_fbo_width = width;
	tracegrid_fbo_height = height;

	return TRUE;
}


int tracegrid_init(void){
	return TRUE;
}






//todo multi contexts
int tracegrid_initOtherContext(void){
	//currently nothing here
	//set up vbo
	tracegrid_vao.type = 1; //trick vbo manager to thinking it is tracked
	//set up strides
	tracegrid_vao.datawidth[0] = 2; // pos
	tracegrid_vao.datawidth[2] = 2; // texcoord
	if(vbo_setup(&tracegrid_vao) != 3) {
		printf("TRACEGRID/tracegrid_init error VBO failed to initialize\n");
		return FALSE;
	}

	tracegrid_resize(100, 100);




	tracegrid_debuggridshader_id = shader_register("shaders/debuggrid.program");
	shader_t *s = shader_returnById(tracegrid_debuggridshader_id);
	shader_load(s);

	tracegrid_firstbounceshader_id = shader_register("shaders/firstbounce.program");
	s = shader_returnById(tracegrid_firstbounceshader_id);
	shader_load(s);

	tracegrid_debugfirstbounceshader_id = shader_register("shaders/debugfirstbounce.program");
	s = shader_returnById(tracegrid_debugfirstbounceshader_id);
	shader_load(s);

	tracegrid_debuggridminishader_id = shader_register("shaders/debuggridmini.program");
	s = shader_returnById(tracegrid_debuggridminishader_id);
	shader_load(s);

	tracegrid_initFramebuffer(800, 600); //todo
	return TRUE;
}

//REMADE AFTER DEC 2
//TODO just copy what i did in planegrid here
int tracegrid_resize(int width, int height){
	if(width < 2){
		printf("TRACEGRID/tracegrid_resize warning width is < 2, not resizing\n");
		return 0;
	}
	if(height < 2){
		printf("TRACEGRID/tracegrid_resize warning height is < 2, not resizing\n");
		return 0;
	}
	if(width == tgwidth && height == tgwidth){
		return 0;
	}
//	printf("TRACEGRID/tracegrid_resize resizing tracegrid to %i by %i\n", width, height);
	//generate temp stuff
	GLfloat * texcoords = malloc(width * height * 2 * sizeof(GLfloat));
	GLfloat * positions = malloc(width * height * 2 * sizeof(GLfloat));
	int x, y;
	for(y = 0; y < height; y++){
		float tcy = (float)y/(float)(height-1);
		float posy = 2*tcy-1.0;
		GLfloat * texcoordsrow = texcoords + 2*y*width;
		GLfloat * positionsrow = positions + 2*y*width;
		for(x = 0; x < width; x++){
			float tcx = (float)x/(float)(width-1);
			float posx = 2*tcx-1.0;
			texcoordsrow[x*2 + 0] = tcx;
			texcoordsrow[x*2 + 1] = tcy;
			positionsrow[x*2 + 0] = posx;
			positionsrow[x*2 + 1] = posy;
		}
	}


	int squareswidth = width-1;
	int squaresheight = height-1;

	//no stuff like shorts here (maybe do if we are running out of memory bandwidth?)
	GLuint * indices = malloc(squaresheight * squareswidth * 6 * sizeof(GLuint));
	for(y = 0; y < squaresheight; y++){
		GLuint * indicesrow = indices + 6*y*squareswidth;
		for(x = 0; x < squareswidth; x++){
			indicesrow[x*6 + 0] = y     * width + x;
			indicesrow[x*6 + 1] = (y+1) * width + x;
			indicesrow[x*6 + 2] = y     * width + x+1;

			indicesrow[x*6 + 3] = y     * width + x+1;
			indicesrow[x*6 + 4] = (y+1) * width + x;
			indicesrow[x*6 + 5] = (y+1) * width + x+1;
		}
	}
	glBindVertexArray(tracegrid_vao.vaoid);

	glBindBuffer(GL_ARRAY_BUFFER, tracegrid_vao.vertsid[0]);
	glBufferData(GL_ARRAY_BUFFER, width * height * 2 * sizeof(GLfloat), positions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tracegrid_vao.vertsid[2]);
	glBufferData(GL_ARRAY_BUFFER, width * height * 2 * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tracegrid_vao.facesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, squareswidth * squaresheight * 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);
	CHECKGLERROR

	tgwidth = width;
	tgheight = height;
	tgtriangles = squareswidth * squaresheight * 2;

	if(texcoords) free(texcoords); //TODO do i want to keep these around?
	if(positions) free(positions); //TODO do i want to keep these around?
	if(indices) free(indices);

	tracegrid_vao.numfaces = tgtriangles;
	tracegrid_vao.numverts = width * height;

//	resultsbuffer = realloc(resultsbuffer, width * height * 2 * sizeof(GLfloat));
	return TRUE;
}



int tracegrid_renderDebugGrid(viewport_t * caster, viewport_t * v){
	if(!v || !v->type){
		printf("TRACEGRID/renderDebugGrid: ERROR invalid viewport!\n");
		return 0;
	}
	if(!caster || !caster->type){
		printf("TRACEGRID/renderDebugGrid: ERROR invalid caster viewport!\n");
		return 0;
	}
	glBindTexture(GL_TEXTURE_2D, tracegrid_fbo_postex);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(tracegrid_vao.vaoid);
	CHECKGLERROR

	shader_t *s = shader_returnById(tracegrid_debuggridshader_id);
	glUseProgram(s->programid);
	CHECKGLERROR
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&caster->viewprojinv, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	Matrix4x4_ToArrayFloatGL(&v->viewproj, tmat);
	glUniformMatrix4fv(s->uniloc[1], 1, GL_FALSE, tmat);
	glUniform1i(s->uniloc[2], 0);
	CHECKGLERROR

	//printf("%i triangles\n", tracegrid_vao.numfaces);

	glDrawElements(GL_TRIANGLES, tracegrid_vao.numfaces * 3, GL_UNSIGNED_INT, 0);

	glUniform1i(s->uniloc[2], 1);
	glDrawElements(GL_TRIANGLES, tracegrid_vao.numfaces * 3, GL_UNSIGNED_INT, 0);
	return TRUE;
}

int tracegrid_renderDebugGridMini(viewport_t * caster, viewport_t *v){
	if(!v || !v->type){
		printf("TRACEGRID/renderDebugGridMini: ERROR invalid viewport!\n");
		return 0;
	}
	if(!caster || !caster->type){
		printf("TRACEGRID/renderDebugGridMini: ERROR invalid caster viewport!\n");
		return 0;
	}
//	glBindTexture(GL_TEXTURE_2D, tracegrid_fbo_postex);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(tracegrid_vao.vaoid);
	CHECKGLERROR

	shader_t *s = shader_returnById(tracegrid_debuggridminishader_id);
	glUseProgram(s->programid);
	CHECKGLERROR


	matrix4x4_t tmath;
	float tmat[16];
	float box = caster->aspect;
	float view = v->aspect;
	float scale = box > view ? 0.5/box : 0.5/view;
	Matrix4x4_CreateTranslate(&tmath, scale * box - 1.0, scale * view - 1.0, 0.0);
	Matrix4x4_ConcatScale3(&tmath, scale * box, scale * view, 1.0);
	Matrix4x4_ToArrayFloatGL(&tmath, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);


	//printf("%i triangles\n", tracegrid_vao.numfaces);

	glDrawElements(GL_TRIANGLES, tracegrid_vao.numfaces * 3, GL_UNSIGNED_INT, 0);
	return TRUE;
}

int tracegrid_renderDebugFirstbounce(viewport_t *caster, viewport_t *v){
	if(!v || !v->type){
		printf("TRACEGRID/renderDebugGridFirstBounce: ERROR invalid viewport!\n");
		return 0;
	}
	if(!caster || !caster->type){
		printf("TRACEGRID/renderDebugGridFirstBounce: ERROR invalid caster viewport!\n");
		return 0;
	}
	glBindTexture(GL_TEXTURE_2D, tracegrid_fbo_postex);
	glBindVertexArray(tracegrid_vao.vaoid);
	CHECKGLERROR

	shader_t *s = shader_returnById(tracegrid_debugfirstbounceshader_id);
	glUseProgram(s->programid);
//	glUniform4fv(s->uniloc[0], 1, resize);
	CHECKGLERROR
	matrix4x4_t tmath;
	Matrix4x4_Concat(&tmath, &v->viewproj, &caster->viewprojinv);
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&tmath, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	fsquad_render();
	glDisable(GL_DEPTH_TEST);

	//figure out how big we wanna render it
	float box = caster->aspect;
	float view = v->aspect;
	float scale = box > view ? 0.5/box : 0.5/view;
	Matrix4x4_CreateTranslate(&tmath, scale * box - 1.0, scale * view - 1.0, 0.0);
	Matrix4x4_ConcatScale3(&tmath, scale * box, scale * view, 1.0);
	Matrix4x4_ToArrayFloatGL(&tmath, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
//	CHECKGLERROR

	//printf("%i triangles\n", tracegrid_vao.numfaces);

//	glDrawElements(GL_TRIANGLES, tracegrid_vao.numfaces * 3, GL_UNSIGNED_INT, 0);
	fsquad_render();
	return TRUE;
}




int tracegrid_trace(void){
	//oh boy
	//todo tile, simd, or other improvments
	//note can just do check here if it falls
//	int x, y;
//	for(y = 0; y < height; y++){
//		for(x = 0; x < width; x++){
			//get ready for spaghetti
			//returns TRUE if fell on the real screen
			//if(do_trace(&pointah, &pointah)) update_VST_BB;
//		}
//	}
	return TRUE;
}
/*
int tracegrid_pushresults(void){
	glBindVertexArray(tracegrid_vao.vaoid);
	glBindBuffer(GL_ARRAY_BUFFER, tracegrid_vao.vertsid[0]);
	glBufferData(GL_ARRAY_BUFFER, tgwidth * tgheight * 2 * sizeof(GLfloat), resultsbuffer, GL_STREAM_DRAW); // temporarily filling with the texcoords, will later fill with traced results, may change this.
	return TRUE;
}
*/
int tracegrid_shutdown(void){
	//probably remove the vbo
	vbo_unload(&tracegrid_vao);
	return TRUE;
}
