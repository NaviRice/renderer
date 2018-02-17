#ifndef BBOXRENDERERHEADER
#define BBOXRENDERERHEADER




int bboxrenderer_init(void);
int bboxrenderer_shutdown(void);


int bboxrenderer_renderBBox(viewport_t *v, matrix4x4_t *m);



#endif
