#ifndef GLMANAGERHEADER
#define GLMANAGERHEADER

int gl_init(void);
int gl_renderWorldDebug(double time);
int gl_renderWorld(double time);
int gl_renderDebug(double time);
int gl_renderFirstbounce(double time);

int gl_resize(int width, int height);


int gl_shutdown(void);

int gl_renderFrame(double time); //temp

void gl_printError(int errornumber, const char *filename, int line);
int gl_resizeViewports(int width, int height);
int gl_resizeDebugViewports(int width, int height);

#define CHECKGLERROR {if (FALSE){if (FALSE) printf("CHECKGLERROR at %s:%d\n", __FILE__, __LINE__);int checkglerror_errornumber = glGetError();if (checkglerror_errornumber) gl_printError(checkglerror_errornumber, __FILE__, __LINE__);}}


#endif
