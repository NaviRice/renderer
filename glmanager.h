#ifndef GLMANAGERHEADER
#define GLMANAGERHEADER

int gl_init(void);
int gl_renderFrame(void);
int gl_renderDebug(void);

int gl_resize(int width, int height);


int gl_shutdown(void);

int gl_renderFrame(void); //temp

void gl_printError(int errornumber, const char *filename, int line);
#define CHECKGLERROR {if (TRUE){if (FALSE) printf("CHECKGLERROR at %s:%d\n", __FILE__, __LINE__);int checkglerror_errornumber = glGetError();if (checkglerror_errornumber) gl_printError(checkglerror_errornumber, __FILE__, __LINE__);}}


#endif
