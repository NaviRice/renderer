#ifndef FIRSTBOUNCERENDERERHEADER
#define FIRSTBOUNCERENDERERHEADER


int firstbouncerenderer_init(void);
int firstbouncerenderer_shutdown(void);

int firstbounce_renderModel(viewport_t *v, int mid, matrix4x4_t *mat);


typedef struct deferredModelUBOStruct_s {
	GLfloat vp[16];
	GLfloat m[16];
	GLfloat v[16];
} deferredModelUBOStruct_t;
typedef struct deferredModelCallbackData_s {
	unsigned int modelid;
	int uboDataOffset;

	deferredModelUBOStruct_t u;
} deferredModelCallbackData_t;


#endif
