#include <GLFW/glfw3.h>

#include "globaldefs.h"
#include "glfwmanager.h"

//temp
extern int glResizeViewports(int width, int height);


GLFWwindow * window;
GLFWwindow * window2;


const char ** glfw_extensions = 0;
unsigned int glfw_extensions_count = 0;


int glfwerror = 0;

int winwidth = 0;
int winheight = 0;

int win2width = 0;
int win2height = 0;



int glfw_resizeWindow(int width, int height, int bpp, int debugmode){
//	if(debugmode) console_printf("DEBUG -- GLFW video resize to: %ix%i width who gives a fuck about bpp i dont handle it\n", width, height);
	if(debugmode) printf("DEBUG -- GLFW video resize to: %ix%i width who gives a fuck about bpp i dont handle it\n", width, height);
	if(height <1) height =1;
	if(width <1) width =1;
	glfwSetWindowSize(window, width, height);
//	int r = glResizeViewports(width, height);
//	if(!r){
//		console_printf("ERROR -- GL framebuffers resize failed\n");
//		return FALSE;
//	}
	winwidth = width;
	winheight = height;
	return TRUE;
}

int glfw_resizeWindow2(int width, int height, int bpp, int debugmode){
//	if(debugmode) console_printf("DEBUG -- GLFW video resize to: %ix%i width who gives a fuck about bpp i dont handle it\n", width, height);
	if(debugmode) printf("DEBUG -- GLFW video resize to: %ix%i width who gives a fuck about bpp i dont handle it\n", width, height);
	if(height <1) height =1;
	if(width <1) width =1;
	glfwSetWindowSize(window2, width, height);
//	int r = glResizeViewports(width, height);
//	if(!r){
//		console_printf("ERROR -- GL framebuffers resize failed\n");
//		return FALSE;
//	}
	win2width = width;
	win2height = height;
	return TRUE;
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
	window = glfwCreateWindow(width, height, "Main Screen", NULL, NULL);
	if(!window){
		glfwTerminate();
		printf("ERROR -- GLFW window creation failed\n");
		return FALSE;
	}
	if(!glfw_resizeWindow(width, height, bpp, debugmode)) return FALSE;
	window2 = glfwCreateWindow(width, height, "Debug Screen", NULL, window);

	if(!window2){
		glfwTerminate();
		printf("ERROR -- GLFW window 2 creation failed\n");
		return FALSE;
	}

	if(!glfw_resizeWindow2(width, height, bpp, debugmode)) return FALSE;
	glfwMakeContextCurrent(window);//todo figure this shit out
//	glfwMakeContextCurrent(window2);
	return TRUE;
}

void glfw_context1(void){
	glfwMakeContextCurrent(window);
}
void glfw_context2(void){
	glfwMakeContextCurrent(window2);
}

void glfw_swapBuffers(void){
	glfwSwapBuffers(window);
}
void glfw_swapBuffers2(void){
	glfwSwapBuffers(window2);
}

int glfw_checkEvent(void){
	//todo
	glfwPollEvents();
	if(glfwWindowShouldClose(window) || glfwWindowShouldClose(window2)){
		glfwTerminate();
		exit(0);
	}
	int iw, ih;
	glfwGetWindowSize(window, &iw, &ih);
	if(iw != winwidth || ih != winheight) glfw_resizeWindow(iw, ih, 24, 0);
	glfwGetWindowSize(window2, &iw, &ih);
	if(iw != win2width || ih != win2height) glfw_resizeWindow2(iw, ih, 24, 0);
	return TRUE;
}


int glfw_shutdown(void){
	//TODO so much TODO
	printf("glfw shutting down\n");
	glfwTerminate();
	return TRUE;
}


