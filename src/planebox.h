#ifndef PLANEBOXHEADER
#define PLANEBOXHEADER


typedef struct planebox_s {
	int type;
	matrix4x4_t model; // debug mode view, and going from planeboxspace back to world
	matrix4x4_t modelrotonly; // normals from planeboxspace back to world
	matrix4x4_t frusty;	//world to planeboxspace
//	plane_t *p;
//	vec3_t center;
	float width;
	float height;
	float depth;//computed
	float prescale;
	char * name;
	vec3_t loc;
	vec3_t rot;
	//todo bboxp

	char * depthname;
	int depthwidth;
	int depthheight;
	float * depthdata;
	GLuint depthtexid;

	char * normname;
	int normwidth;
	int normheight;
	float * normdata;
	GLuint normtexid;
	vbo_t thevbo; //generated for each, because differing resolutions


	int flat;
}planebox_t;



vbo_t planebox_vao;
int planebox_renderDebug(planebox_t *p, viewport_t *v);
int planebox_renderFirstbounce(planebox_t *p, viewport_t *v);

int planebox_renderDebugLines(planebox_t *p, viewport_t *v);
int planebox_renderViewportDebugLines(viewport_t *debug, viewport_t *v);

int planebox_shutdown(void);
int planebox_init(void);

int planebox_parsePlaneboxFIle(planebox_t *p);

int planebox_load(planebox_t *p);

#endif
