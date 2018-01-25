#include <GLFW/glfw3.h>
#include "globaldefs.h"
#include "contextmanager.h"


mycontext_t mycontexts[NUMCONTEXTS] = {0};
int context_current = -1;



int context_switch(int c){
	if(c == context_current) return 1;
	if(c > NUMCONTEXTS || c < 0) return 0;

	//different c, valid one
	glfwMakeContextCurrent(mycontexts[c].window);
	context_current = c;
	return 2;
}

