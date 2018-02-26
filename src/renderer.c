//Sys includes

//local includes
#include "globaldefs.h"
#include "renderer.h"
//#include "glmanager.h"

#include "glfwmanager.h" //init
#include "glmanager.h" //init

#include "cvarmanager.h"

#include "matrixlib.h"
#include "entitymanager.h" //init
#include "contextmanager.h"
#include "headclient.h"
#include "navmanager.h"

#include "gamecodemanager.h"

	extern double glfwGetTime(void);

int shutitdown(){
	printf("Shutting down\n");
	gl_shutdown();
	glfw_shutdown();
	entity_shutdown();
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
	if(!gamecode_init()){printf("Unable to init gamecode\n"); shutitdown(); return 4;}
	//todo move to gamecode but whatever
	headclient_init();
	nav_init();




	double t, to;

	to=glfwGetTime();

	double timesincelastfpsupdate = 0.0;
	int framecount = 0;
	double accum = 0.0;


	while(TRUE){		//temp
		t = glfwGetTime();
		double delta = t-to;
		to = t;
		timesincelastfpsupdate+=delta;
		if(timesincelastfpsupdate > 5.0){
			printf("%f fps: %i frames in %f seconds: %f ms per frame\n", (float)framecount/(float)timesincelastfpsupdate, framecount,
				timesincelastfpsupdate, 1000.0*(float)timesincelastfpsupdate / (float) framecount);
			framecount = 0;
			timesincelastfpsupdate = 0;
		}
		framecount++;

		accum += delta;
		//grab most recent head

		glfw_checkEvent();
		//todo figure out if thise should be reordered
		headclient_update(t);
		while(accum * 1000.0 > GCTIMESTEP){
			gamecode_tick();
			accum -= (double)GCTIMESTEP/1000.0;
		}
//		headclient_update();


		//todo recalc viewport stuff here
		context_switch(0);
		gl_renderWorld(t);
		gl_renderFirstbounce(t);
		gl_renderFrame(t);
		glfw_swapBuffers();
		context_switch(1);
		//todo move to context 1 once i verify that it worky
		gl_renderDebug(t);
		glfw_swapBuffers();


	}
	shutitdown();
	return FALSE;
}
