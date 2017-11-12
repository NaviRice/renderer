//Sys includes

//local includes
#include "globaldefs.h"
#include "renderer.h"
//#include "glmanager.h"

#include "glfwmanager.h" //init
#include "glmanager.h" //init

#include "cvarmanager.h"

#include "matrixlib.h"
//#include "entitymanager.h" //init


int shutitdown(){
	printf("Shutting down\n");
	gl_shutdown();
	glfw_shutdown();
//	entity_shutdown();
	cvar_shutdown();
	//todo
	return FALSE;
}
cvarcallback_t CVAR_test_callbacks[] = {cvar_print, 0};
cvar_t CVAR_test = {CVAR_SAVEABLE, "test", "testing help text", "53.5"};

int main(int argc, char *argv[]){
	if(!cvar_init()){printf("Unable to init cvar\n"); shutitdown(); return 1;}

	cvar_register(&CVAR_test);

	cvar_t *ctest = cvar_findByNameRPOINT("test");
	cvar_print(ctest);

	CVAR_test.onchanges = CVAR_test_callbacks;

	cvar_set("test", "127.1 101.0 53.2222");

	cvar_unregister(ctest->myid);

//	if(!entity_init()){printf("Unable to init entity\n"); shutitdown(); return 4;}

	if(!glfw_init(800, 600, 24,1)){printf("Unable to init glfw\n"); shutitdown(); return 2;}
	if(!gl_init()){printf("Unable to init gl\n"); shutitdown(); return 3;}


	while(TRUE){		//temp
		gl_renderFrame();
		glfw_swapBuffers();
	}
	shutitdown();
	return FALSE;
}
