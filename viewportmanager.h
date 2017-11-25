#ifndef VIEWPORTHEADER
#define VIEWPORTHEADER

typedef struct viewport_s {
	int type;
	int myid;
//	char * name; //dont think im gonna name em
	float aspect;
	float fov;
	float near;
	float far;
	int changed;			//bit in 1s place is view changed, bit in 2s place is  projection changed

	//FBO stuff here?
	vec3_t pos;
	vec3_t angle;
	matrix4x4_t projection;
	matrix4x4_t view;
	matrix4x4_t viewproj;
	matrix4x4_t cam;		//basically the same as an entity matrix
	plane_t frustum[6];
	vec_t bboxp[24];

	vec3_t v_forward;
	vec3_t v_up;
	vec3_t v_right;
} viewport_t;
//todo
extern viewport_t viewport_default;




#include "idlist.h"
IDLIST_HEADER_NOHASH(viewport, viewport_t);


int viewport_init(void);

int viewport_recalc(viewport_t *v);

#endif
