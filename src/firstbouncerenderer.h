#ifndef FIRSTBOUNCERENDERERHEADER
#define FIRSTBOUNCERENDERERHEADER


int firstbouncerenderer_init(void);
int firstbouncerenderer_shutdown(void);

int firstbounce_renderModel(viewport_t *v, int mid, matrix4x4_t *mat);


typedef struct deferredModelCallbackData_s {
	unsigned int modelid;
	unsigned int shaderid; //do i need this?
	int uboDataOffset;

	float mvp[16];
	float m[16];
	float v[16];
} deferredModelCallbackData_t;


#endif
