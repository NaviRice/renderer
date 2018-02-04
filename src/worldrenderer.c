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



//todo move this shit to a framebuffer manager
GLuint worldrenderer_fbo[NUMCONTEXTS] = {0};
GLuint worldrenderer_fbo_postex = 0;
GLuint worldrenderer_fbo_renderbuffer = 0;

int worldrenderer_fbo_height = 0;
int worldrenderer_fbo_width = 0;


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

	worldrenderer_initFramebufferonly();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return TRUE;
}


int worldrenderer_init(void){
	worldrenderer_initFramebuffer(1024, 1024);
	//TODO
	return TRUE;
}

int worldrenderer_shutdown(void){
	//TODO
	//todo framebuffer stuff
	return TRUE;
}




int worldrenderer_recalcFakeVP(viewport_t *v){ //todo change this to also do shrinking based on windshield bboxp
	int i;
	for(i =0 ; i <= entity_arraylasttaken; i++){
		entity_t *e = &entity_list[i];
		if(!e->myid)continue;
	}
	//im probably going to recalc fov, aspect, near, far, angle
	//pos stays the same
	return TRUE;
}

int worldrenderer_renderEntities(viewport_t *v){
	int i;
	int counter = 0;
	for(i=0 ; i <= entity_arraylasttaken; i++){
		entity_t *e = &entity_list[i];
		if(!e->myid)continue;
		counter++;
	}
	return counter;
}
