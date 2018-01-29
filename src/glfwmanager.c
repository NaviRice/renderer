#include <GLFW/glfw3.h>

#include "globaldefs.h"

#include "glfwmanager.h"
#include "contextmanager.h"
//temp
extern int gl_resizeViewports(int width, int height);
extern int gl_resizeDebugViewports(int width, int height);



const char ** glfw_extensions = 0;
unsigned int glfw_extensions_count = 0;


int glfwerror = 0;

//do i need current context for this?
int glfw_resizeWindow(int con, int width, int height, int bpp, int debugmode){
	if(debugmode) printf("DEBUG -- GLFW video resize to: %ix%i\n", width, height);
	if(height <1) height =1;
	if(width <1) width =1;
	mycontext_t *c = mycontexts+con;
	glfwSetWindowSize(c->window, width, height);
//not used anymore
/*	int r = gl_resizeViewports(width, height);
	if(!r){
		printf("ERROR -- GL framebuffers resize failed\n");
		return FALSE;
	}
*/
	c->width = width;
	c->height = height;
	c->changed = TRUE;
	return TRUE;
}
int glfw_resizeWindow_current(int width, int height, int bpp, int debugmode){
	return glfw_resizeWindow(context_current, width, height, bpp, debugmode);
}

void glfw_handleError(int error, const char * desc){
	//console_printf("ERROR -- GLFW %i-- %s\n", error, desc);
	printf("ERROR -- GLFW %i-- %s\n", error, desc);
	glfwerror = error;
}


int glfw_init(int width, int height, int bpp, int debugmode){
//	if(debugmode) console_printf("DEBUG -- GLFW window init\n");
	if(debugmode) printf("DEBUG -- GLFW window init\n");
	glfwSetErrorCallback(glfw_handleError);
	if(!glfwInit()){
		printf("ERROR -- GLFW init failed\n");
		return FALSE;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);

	int i;
	for(i = 0; i < NUMCONTEXTS; i++){
		mycontext_t *c = mycontexts+i;
		c->changed = TRUE;
		c->window = glfwCreateWindow(width, height, "Main Screen", NULL, i ? mycontexts[0].window : NULL);
		if(!c->window){
			glfwTerminate();
			printf("ERROR -- GLFW window creation failed for context %i\n", i);
			return FALSE;
		}
		if(!context_switch(i)){
			glfwTerminate();
			printf("ERROR -- GLFW context switch failed for context %i", i);
			return FALSE;
		}
		if(!glfw_resizeWindow_current(width, height, bpp, debugmode)){
			glfwTerminate();
			printf("ERROR -- GLFW window resize failed for context %i\n", i);
			return FALSE;
		}
	}
	context_switch(0);
	return TRUE;
}

void glfw_swapBuffers(void){//todo check if i can just set this to an input int
	glfwSwapBuffers(mycontexts[context_current].window);
}

int glfw_checkEvent(void){
	glfwPollEvents();
	//todo
	int i, iw, ih;
	for(i = 0; i < NUMCONTEXTS; i++){
		mycontext_t *c = mycontexts+i;
		if(glfwWindowShouldClose(c->window)){
			glfwTerminate();
			exit(0);
		}
		glfwGetWindowSize(c->window, &iw, &ih);
		if(iw != c->width || ih != c->height)glfw_resizeWindow(i, iw, ih, 24, 1);
	}
	return TRUE;
}


int glfw_shutdown(void){
	//TODO so much TODO
	printf("glfw shutting down\n");
	glfwTerminate();
	return TRUE;
}
