#include <epoxy/gl.h>
#include <epoxy/glx.h>
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


//generate a bbox vao
int planebox_init(void){
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



	//load shader now
	planebox_lineshader_id = shader_register("shaders/line.program");
	shader_t *s = shader_returnById(planebox_lineshader_id);
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
		}
	}
	file_close(&f);

	p->type = 1; //retundant? check shader as well
	return FALSE;
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
	Matrix4x4_CreateFromQuakeEntity3(&p->model, p->width, p->height, p->depth, p->rot[0], p->rot[1], p->rot[2], p->width/2.0, p->height/2.0, p->depth);
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
//		planebox_load
			p->depth = 1.0; //tmp
			p->type = 3;
		case 3:
			planebox_genMats(p);
			p->type = 4;
		default: return p->type;
	}
}
//todo
//int planebox_unload(planebox_t *p)


int planebox_renderDebug(planebox_t *p, viewport_t *v){
	if(!p || !p->type){
		printf("PLANEBOX/renderDebug: ERROR invalid planebox!\n");
		return 0;
	}
//	printf("planebox vao %i\n", planebox_vao.vaoid);
	glBindVertexArray(0);
	CHECKGLERROR
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
