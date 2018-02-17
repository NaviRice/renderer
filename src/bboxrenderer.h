#ifndef BBOXRENDERERHEADER
#define BBOXRENDERERHEADER




int bboxrenderer_init(void);
int bboxrenderer_shutdown(void);



int bboxrenderer_renderBBox(viewport_t *v, vec_t bbox[6]);
int bboxrenderer_renderBBoxPCustom(viewport_t *v, vec_t bboxp[24]);
int bboxrenderer_renderBBoxMat(viewport_t *v, matrix4x4_t *m);
int bboxrenderer_renderBBoxTmat(float tmat[16]);



#endif
