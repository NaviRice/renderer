#include <epoxy/gl.h>
#include "glmanager.h" // for CHECKGLERROR
#include "globaldefs.h"

#include "matrixlib.h"
#include "viewportmanager.h"
#include "tracegrid.h"

#include "shadermanager.h"
#include "vbomanager.h"

vbo_t tracegrid_vao = {0};
int tgwidth = 0;
int tgheight = 0;
int tgtriangles = 0;

int tracegrid_debuggridshader_id = 0;


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
		printf("TRACEGRID/renderDebugGrid: ERROR invaliud viewport!\n");
		return 0;
	}
	if(!caster || !caster->type){
		printf("TRACEGRID/renderDebugGrid: ERROR invalid caster viewport!\n");
		return 0;
	}
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(tracegrid_vao.vaoid);
	CHECKGLERROR

	shader_t *s = shader_returnById(tracegrid_debuggridshader_id);
	glUseProgram(s->programid);
	CHECKGLERROR
	matrix4x4_t tmath;
	Matrix4x4_Concat(&tmath, &v->viewproj, &caster->viewprojinv);
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&tmath, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);
	CHECKGLERROR

	//printf("%i triangles\n", tracegrid_vao.numfaces);

	glDrawElements(GL_TRIANGLES, tracegrid_vao.numfaces * 3, GL_UNSIGNED_INT, 0);
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
