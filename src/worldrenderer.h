#ifndef WORLDRENDERERHEADER
#define WORLDRENDERERHEADER




int worldrenderer_init(void);
int worldrenderer_shutdown(void);




GLuint worldrenderer_fbo_postex; //needed for when rendering the tracegrid to the actual screen

int worldrenderer_renderFakeVP(viewport_t *v);

int worldrenderer_bindEASTEREGG(void);


int worldrenderer_addEntitiesToQueue(renderqueue_t *q, viewport_t *v);

int worldrenderer_renderEntities(viewport_t *v);
int worldrenderer_renderEntitiesBBoxes(viewport_t *v);
int worldrenderer_renderDebugFramebufferMini(viewport_t * caster, viewport_t *v);

int worldrenderer_renderModel(viewport_t *v, int mid, matrix4x4_t *m, vec4_t color);


int worldrenderer_recalcFakeVP(viewport_t *v);

#endif
