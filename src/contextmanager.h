#ifndef CONTEXTHEADER
#define CONTEXTHEADER
#define NUMCONTEXTS 2

typedef struct mycontext_s {
#ifdef _glfw3_h_
	GLFWwindow * window;
#else
	void * window;
#endif
	int width;
	int height;
} mycontext_t;

mycontext_t mycontexts[NUMCONTEXTS];

int context_current;


int context_switch(int c);


#endif
