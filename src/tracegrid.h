#ifndef TRACEGRIDHEADER
#define TRACEGRIDHEADER


int tracegrid_init(void);
int tracegrid_initOtherContext(void);

int tracegrid_resize(int width, int height);
int tracegrid_shutdown(void);


int tracegrid_renderDebugGrid(viewport_t *view, viewport_t *debug);


#endif
