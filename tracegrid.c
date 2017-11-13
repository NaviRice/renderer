#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include "globaldefs.h"
#include "tracegrid.h"

#include "vbomanager.h"

vbo_t tgvbo;
int tgwidth = 0;
int tgheight = 0;

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
	return TRUE;
}


int tracegrid_resize(int width, int height){
	return FALSE;
}

int tracegrid_shutdown(void){
	//probably remove the vbo
	vbo_unload(&tgvbo);
	return TRUE;
}
