#include <epoxy/gl.h>
#include "glmanager.h" // for CHECKGLERROR
#include "globaldefs.h"
#include "tracegrid.h"

#include "vbomanager.h"

vbo_t tgvbo;
int tgwidth = 0;
int tgheight = 0;
int tgtriangles = 0;

GLfloat *resultsbuffer = 0;

//using an untracked vbo

int tracegrid_init(void){
	//currently nothing here
	//set up vbo
	tgvbo.type = 1; //trick vbo manager to thinking it is tracked
	//set up strides
	tgvbo.datawidth[0] = 2; // pos
	tgvbo.datawidth[1] = 2; // texcoord
	if(vbo_setup(&tgvbo) != 3) {
		printf("TRACEGRID/tracegrid_init error VBO failed to initialize\n");
		return FALSE;
	}

	tracegrid_resize(100, 100);
	return TRUE;
}

//UNTESTED
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
	int x, y;
	for(y = 0; y < height; y++){
		float tcy = (float)y/(float)height;
		GLfloat * texcoordsrow = texcoords + 2*y*width;
		for(x = 0; x < width; x++){
			float tcx = (float)x/(float)width;
			texcoordsrow[x*2 + 0] = tcx;
			texcoordsrow[x*2 + 1] = tcy;
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
	glBindVertexArray(tgvbo.vaoid);

	glBindBuffer(GL_ARRAY_BUFFER, tgvbo.vertsid[0]);
	glBufferData(GL_ARRAY_BUFFER, width * height * 2 * sizeof(GLfloat), texcoords, GL_STREAM_DRAW); // temporarily filling with the texcoords, will later fill with traced results, may change this.

	glBindBuffer(GL_ARRAY_BUFFER, tgvbo.vertsid[1]);
	glBufferData(GL_ARRAY_BUFFER, width * height * 2 * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tgvbo.facesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, squareswidth * squaresheight * 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);
	CHECKGLERROR

	tgwidth = width;
	tgheight = height;
	tgtriangles = squareswidth * squaresheight * 2;

	if(texcoords) free(texcoords); //TODO do i want to keep these around?
	if(indices) free(indices);

	resultsbuffer = realloc(resultsbuffer, width * height * 2 * sizeof(GLfloat));
	return TRUE;
}


int tracegrid_trace(void){
	//oh boy
	//todo tile, simd, or other improvments
	//note can just do check here if it falls
	int x, y;
//	for(y = 0; y < height; y++){
//		for(x = 0; x < width; x++){
			//get ready for spaghetti
			//returns TRUE if fell on the real screen
			//if(do_trace(&pointah, &pointah)) update_VST_BB;
//		}
//	}
}

int tracegrid_pushresults(void){
	glBindVertexArray(tgvbo.vaoid);
	glBindBuffer(GL_ARRAY_BUFFER, tgvbo.vertsid[0]);
	glBufferData(GL_ARRAY_BUFFER, tgwidth * tgheight * 2 * sizeof(GLfloat), resultsbuffer, GL_STREAM_DRAW); // temporarily filling with the texcoords, will later fill with traced results, may change this.
	return TRUE;
}

int tracegrid_shutdown(void){
	//probably remove the vbo
	vbo_unload(&tgvbo);
	return TRUE;
}
