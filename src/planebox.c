#include <epoxy/gl.h>
#include "globaldefs.h"

#include "matrixlib.h"
#include "viewportmanager.h"
#include "vbomanager.h"
#include "glmanager.h"


#include "stringlib.h"
#include "filesys.h"

#include "planebox.h"

#include "shadermanager.h"	//todo renderqueue?


vbo_t planebox_vao = {0};
int planebox_lineshader_id = 0;
int planebox_debugmesh_id = 0;

int planebox_shutdown(void){ //todo
	return TRUE;
}

//generate a bbox vao
int planebox_init(void){
	return 1;
}
int planebox_initOtherContext(void){
	planebox_vao.type = 1;	//trick vbo manager into its a legit vao
	planebox_vao.datawidth[0] = 3; //pos
	planebox_vao.datawidth[3] = 3; //color

	if(vbo_setup(&planebox_vao) != 3){
		printf("PLANEBOX/init error vbo setup for lines failedi\n");
		return 0;
	}
//	vbo_setup(&planebox_vao);
	//numfaces will be number of lines
	int numfaces = 12;
	//4 + 8 + 4
	int numverts = 16;
	float posdata[16*3] = {
		-1.0, -1.0,  0.0,	1.0, -1.0,  0.0,	1.0,  1.0,  0.0,	-1.0,  1.0,  0.0,
		-1.0, -1.0,  1.0,	1.0, -1.0,  1.0,	1.0,  1.0,  1.0,	-1.0,  1.0,  1.0,
		-1.0, -1.0,  0.0,	1.0, -1.0,  0.0,	1.0,  1.0,  0.0,	-1.0,  1.0,  0.0,
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
	glBindVertexArray(planebox_vao.vaoid);
	CHECKGLERROR
	glBindBuffer(GL_ARRAY_BUFFER, planebox_vao.vertsid[0]);
	glBufferData(GL_ARRAY_BUFFER, numverts * planebox_vao.datawidth[0] * sizeof(GLfloat), posdata, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, planebox_vao.vertsid[3]);
	//todo update vbomanager to handle ubytes
	glBufferData(GL_ARRAY_BUFFER, numverts * planebox_vao.datawidth[3] * sizeof(GLfloat), coldata, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planebox_vao.facesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numfaces*2 * sizeof(GLuint), indices, GL_STATIC_DRAW);
	CHECKGLERROR
	planebox_vao.numverts = numverts;
	planebox_vao.numfaces = numfaces;



	//load shaders now
	planebox_lineshader_id = shader_register("shaders/line.program");
	shader_t *s = shader_returnById(planebox_lineshader_id);
	shader_load(s);

	planebox_debugmesh_id = shader_register("shaders/debugmesh.program");
	s = shader_returnById(planebox_debugmesh_id);
	shader_load(s);

	return TRUE;
}


int planebox_parsePlaneboxFile(planebox_t *p){
	if(!p->name){
		printf("PLANEBOX/parsePlaneboxFile error planebox doesnt have a name?\n");
		return 0;
	}
	file_t f = file_open(p->name, "r");
	//todo check if file is good (and change shader too)
	char * line;
//	p->loc = {0.0, 0.0, 0.0};
//	p->width = p->height = p->depth = 0.0;
	p->prescale = 1.0;
	for(line = file_getEntireLine(&f); line; free(line), line = file_getEntireLine(&f)){
		char *vd[2] = {0, 0};
		string_splitND(line, ':', vd, 2);
		printf("Varname: \"%s\"\t\tVardata: \"%s\"\n", vd[0], vd[1]);
		if(vd[0] && vd[1]){
			//todo image files
			if(string_testEqualCI(vd[0], "Position") || string_testEqualCI(vd[0], "Pos") || string_testEqualCI(vd[0], "Location") || string_testEqualCI(vd[0], "Loc"))
				string_toVec(vd[1], p->loc, 3);
			else if (string_testEqualCI(vd[0], "Rotation") || string_testEqualCI(vd[0], "Rot"))
				string_toVec(vd[1], p->rot, 3);
			else if (string_testEqualCI(vd[0], "Width"))
				string_toVec(vd[1], &p->width, 1);
			else if (string_testEqualCI(vd[0], "Height"))
				string_toVec(vd[1], &p->height, 1);
			else if (string_testEqualCI(vd[0], "Prescale"))
				string_toVec(vd[1], &p->prescale, 1);
			else if (string_testEqualCI(vd[0], "Normalmap"))
				p->normname = strdup(vd[1]);
			else if (string_testEqualCI(vd[0], "Heightmap"))
				p->depthname = strdup(vd[1]);
		}
	}
	file_close(&f);

	p->type = 1; //retundant? check shader as well
	return FALSE;
}


int planebox_fakeImages(planebox_t *p){
	int width = 1024;
	int height = 1024;
	printf("PLANEBOX/fakeImages faking images lel %i %i\n", width, height);
	float depth = 0.0;

	float * data = malloc(width * height * sizeof(float));
	int x, y;
	for(y = 0; y < height; y++){
		float ybump = ((float)y/(float)height) - 0.5;
		ybump*=ybump;
		float * dataline = data + y*width;
		for(x = 0; x < width; x++){
			float xbump = 2.0 * ((float)x/(float)width) -1.0;
			xbump*=xbump;
			dataline[x] = xbump + ybump;
			if(dataline[x] > depth) depth = dataline[x];
		}
	}
	p->depthwidth = width;
	p->depthheight = height;
	p->depthdata = data;
	p->depth = depth;
	return TRUE;
}

int planebox_normalizeImages(planebox_t *p){
	int i;
	int max = p->depthheight * p->depthwidth;
	for(i = 0; i < max; i++){
		p->depthdata[i] /= p->depth;
	}
	return TRUE;
}

int planebox_loadImages(planebox_t *p){
	//todo set this up lel
	if(!planebox_fakeImages(p)) return FALSE;
	if(p->depthdata){
	//now have to go through and normalize... or do i?
	//might wanna double check with normals
	//if i dont care about normals when im in box space, then its fine
	//at this point i dont care about normals in box space, i grab them at the box location and then only rotate
		if(!planebox_normalizeImages(p)) return FALSE;
		glGenTextures(1, &p->depthtexid);
		glBindTexture(GL_TEXTURE_2D, p->depthtexid);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, p->depthwidth, p->depthheight, 0, GL_RED, GL_FLOAT, p->depthdata);
		//no mipmapping... idk
		// look into depth textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	return TRUE;
}


int planebox_genMeshData(planebox_t *p){
	int maxwidth= 0 , maxheight = 0;
	if(p->depthdata){maxheight = p->depthheight; maxwidth = p->depthwidth;}
	if(p->normdata){if(maxheight < p->normheight) maxheight =  p->normheight; if(maxwidth < p->normwidth) maxwidth = p->normwidth;}

	if(maxwidth < 2 || maxheight < 2){
		printf("PLANEBOX/genMeshData, error texture size too small lel\n");
		return FALSE;
	}

	p->thevbo.type = 1;
	p->thevbo.datawidth[0] = 3;
	p->thevbo.datawidth[2] = 2;
	if(vbo_setup(&p->thevbo)!= 3){
		printf("PLANEBOX/genMeshData, error vbo creation failed\n");
		return 0;
	}
	int numverts = maxwidth * maxheight;
	int numfaces = 2 * (maxwidth -1) * (maxheight -1);
	GLfloat *vertdata = malloc(numverts * 3 * sizeof(GLfloat));
	GLfloat *tcdata = malloc(numverts * 2 * sizeof(GLfloat));
	GLuint *facedata = malloc(numfaces * 3 * sizeof(GLuint));


	//todo convert all the positions to 0.0->1.0 instead of-1.0->1.0
	int x, y;
	for(y = 0; y < maxheight; y++){
		GLfloat *vertlinedata = vertdata + y*maxwidth*3;
		GLfloat *tclinedata = tcdata + y*maxwidth*2;
		GLfloat ytc = (float)y/(float)(maxheight-1);
		GLfloat ypos = 2* ytc - 1.0;
		for(x = 0; x < maxwidth; x++){
			GLfloat xtc = (float)x/(float)(maxwidth-1);
			GLfloat xpos = 2.0* xtc - 1.0;
			vertlinedata[x * 3 +0] = xpos;
			vertlinedata[x * 3 +1] = ypos;
			vertlinedata[x * 3 +2] = 0.0; //lel
			tclinedata[x*2+0] = xtc;
			tclinedata[x*2+1] = ytc;
		}
	}

	//generate grid faces
	GLuint *facedatabump = facedata;
	for(y = 0; y < maxheight-1; y++){
		for(x = 0; x < maxwidth-1; x++){
			GLuint bl = y * maxwidth +x;
			GLuint br = bl+1;
			GLuint tl = bl+maxwidth;
			GLuint tr = tl+1;
			facedatabump[0] = bl;
			facedatabump[1] = br;
			facedatabump[2] = tr;
			facedatabump[3] = bl;
			facedatabump[4] = tr;
			facedatabump[5] = tl;
			facedatabump+= 6;
		}
	}


	glBindVertexArray(p->thevbo.vaoid);
	CHECKGLERROR
	glBindBuffer(GL_ARRAY_BUFFER, p->thevbo.vertsid[0]);
	glBufferData(GL_ARRAY_BUFFER, numverts * p->thevbo.datawidth[0] * sizeof(GLfloat), vertdata, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, p->thevbo.vertsid[2]);
	glBufferData(GL_ARRAY_BUFFER, numverts * p->thevbo.datawidth[2] * sizeof(GLfloat), tcdata, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->thevbo.facesid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numfaces *3 * sizeof(GLuint), facedata, GL_STATIC_DRAW);
	p->thevbo.numverts = numverts;
	p->thevbo.numfaces = numfaces;



	if(vertdata) free(vertdata);
	if(facedata) free(facedata);

	return TRUE;
}


int planebox_genMats(planebox_t *p){
	//have to do it myself since scaling and shit
	//todo figure out this order
	//todo just come up with a Matrix4x4 createquake one that can handle 3
//	Matrix4x4_CreateRotate(&p->model, p->rot[0], 1.0, 0.0, 0.0);
//	Matrix4x4_ConcatRotate(&p->model, p->rot[1], 0.0, 1.0, 0.0);
//	Matrix4x4_ConcatRotate(&p->model, p->rot[2], 0.0, 0.0, 1.0);

//	Matrix4x4_ConcatScale3(&p->model, p->width/2.0, p->height/2.0, p->depth);
//	Matrix4x4_ConcatTranslate(&p->model, p->loc[0], p->loc[1], p->loc[2]);
	Matrix4x4_CreateFromQuakeEntity3(&p->model, p->loc[0], p->loc[1], p->loc[2], p->rot[0], p->rot[1], p->rot[2], p->width/2.0, p->height/2.0, p->depth);
	Matrix4x4_CopyRotateOnly(&p->modelrotonly, &p->model);
	Matrix4x4_Invert_Full(&p->frusty, &p->model);
	return TRUE;
}

//todo have a register and shit
int planebox_load(planebox_t *p){
	switch(p->type){
		case 1:
			planebox_parsePlaneboxFile(p);
			p->type = 2;
		//return 0
		case 2:
			planebox_loadImages(p);
			planebox_genMeshData(p);
//			p->depth = 1.0; //tmp
			p->type = 3;
		case 3:
			planebox_genMats(p);
			p->type = 4;
		default: return p->type;
	}
}
//todo
//int planebox_unload(planebox_t *p)




extern int tracegrid_firstbounceshader_id;
int planebox_renderFirstbounce(planebox_t *p, viewport_t *v){
	if(!p || !p->type){
		printf("PLANEBOX/renderFirstbounce: ERROR invalid planebox!\n");
		return 0;
	}
	if(!p->thevbo.numverts){
		printf("PLANEBOX/renderFirstbounce: ERROR planebox has no verts\n");
		return 0;
	}
	if(!v || !v->type){
		printf("PLANEBOX/renderFirstbounce: ERROR invalid viewport!\n");
		return 0;
	}
	glEnable(GL_DEPTH_TEST);
//	printf("planebox vao %i\n", planebox_vao.vaoid);
	glBindVertexArray(p->thevbo.vaoid);
	CHECKGLERROR
	glBindTexture(GL_TEXTURE_2D, p->depthtexid);
//todo clean up this hack shit
	shader_t *s = shader_returnById(tracegrid_firstbounceshader_id);
	glUseProgram(s->programid);
	CHECKGLERROR
	matrix4x4_t tmath;
	Matrix4x4_Concat(&tmath, &v->viewproj, &p->model);
//	Matrix4x4_Concat(&tmath, &p->model, &v->viewproj);
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&v->viewproj, tmat);
//	Matrix4x4_ToArrayFloatGL(&tmath, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);

//	Matrix4x4_ToArrayFloatGL(&v->viewproj, tmat);
	Matrix4x4_ToArrayFloatGL(&p->model, tmat);
	glUniformMatrix4fv(s->uniloc[1], 1, GL_FALSE, tmat);

	CHECKGLERROR
	glDrawElements(GL_TRIANGLES, p->thevbo.numfaces *3, GL_UNSIGNED_INT, 0);
	CHECKGLERROR
	return TRUE;
}


int planebox_renderDebug(planebox_t *p, viewport_t *v){
	if(!p || !p->type){
		printf("PLANEBOX/renderDebug: ERROR invalid planebox!\n");
		return 0;
	}
	if(!p->thevbo.numverts){
		printf("PLANEBOX/renderDebug: ERROR planebox has no verts\n");
		return 0;
	}
	if(!v || !v->type){
		printf("PLANEBOX/renderDebug: ERROR invalid viewport!\n");
		return 0;
	}
	glEnable(GL_DEPTH_TEST);
//	printf("planebox vao %i\n", planebox_vao.vaoid);
	glBindVertexArray(p->thevbo.vaoid);
	CHECKGLERROR
	glBindTexture(GL_TEXTURE_2D, p->depthtexid);
	shader_t *s = shader_returnById(planebox_debugmesh_id);
	glUseProgram(s->programid);
	CHECKGLERROR
	matrix4x4_t tmath;
	Matrix4x4_Concat(&tmath, &v->viewproj, &p->model);
//	Matrix4x4_Concat(&tmath, &p->model, &v->viewproj);
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&tmath, tmat);
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);

	CHECKGLERROR
	glDrawElements(GL_TRIANGLES, p->thevbo.numfaces *3, GL_UNSIGNED_INT, 0);
	CHECKGLERROR
	return TRUE;
}



int planebox_renderDebugLines(planebox_t *p, viewport_t *v){
	if(!p || !p->type){
		printf("PLANEBOX/renderDebugLines: ERROR invalid planebox!\n");
		return 0;
	}
	if(!v || !v->type){
		printf("PLANEBOX/renderDebugLines: ERROR invalid viewport!\n");
		return 0;
	}
	glEnable(GL_DEPTH_TEST);
//	printf("planebox vao %i\n", planebox_vao.vaoid);
	glBindVertexArray(planebox_vao.vaoid);
	CHECKGLERROR

	shader_t *s = shader_returnById(planebox_lineshader_id);
	glUseProgram(s->programid);
	CHECKGLERROR
	matrix4x4_t tmath;
	Matrix4x4_Concat(&tmath, &v->viewproj, &p->model);
//	Matrix4x4_Concat(&tmath, &p->model, &v->viewproj);
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&tmath, tmat);
	CHECKGLERROR
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);

	CHECKGLERROR
	glDrawElements(GL_LINES, planebox_vao.numfaces *2, GL_UNSIGNED_INT, 0);
	CHECKGLERROR
	return TRUE;
}
int planebox_renderViewportDebugLines(viewport_t * debug, viewport_t *v){
	if(!v || !v->type){
		printf("PLANEBOX/renderViewportDebugLines: ERROR invalid viewport!\n");
		return 0;
	}
	if(!debug || !debug->type){
		printf("PLANEBOX/renderViewportDebugLines: ERROR invalid debug viewport!\n");
		return 0;
	}
	glEnable(GL_DEPTH_TEST);
//	printf("planebox vao %i\n", planebox_vao.vaoid);
	glBindVertexArray(planebox_vao.vaoid);
	CHECKGLERROR

	shader_t *s = shader_returnById(planebox_lineshader_id);
	glUseProgram(s->programid);
	CHECKGLERROR
	matrix4x4_t tmath;
	Matrix4x4_Concat(&tmath, &v->viewproj, &debug->viewprojinv);
//	Matrix4x4_Concat(&tmath, &p->model, &v->viewproj);
	float tmat[16];
	Matrix4x4_ToArrayFloatGL(&tmath, tmat);
	CHECKGLERROR
	glUniformMatrix4fv(s->uniloc[0], 1, GL_FALSE, tmat);

	CHECKGLERROR
	glDrawElements(GL_LINES, planebox_vao.numfaces *2, GL_UNSIGNED_INT, 0);
	CHECKGLERROR
	return TRUE;
}
