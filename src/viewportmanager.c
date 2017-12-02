#include <tgmath.h>			//for sin and others
#include "globaldefs.h"
#include "matrixlib.h"
#include "viewportmanager.h"
#include "mathlib.h"			//for all the dist and dot products



IDLIST_INTERNAL_NOHASH(viewport, viewport_t);

int viewport_init(void){
	IDLIST_INIT_NOHASH(viewport, viewport_t);
	return TRUE;
}


vec3_t stockv_forward =	{0.0, 0.0, 1.0};
vec3_t stockv_up =	{0.0, 1.0, 0.0};
vec3_t stockv_right =	{1.0, 0.0, 0.0};

//const inline void recalcViewMatrix(viewport_t *v){
void recalcViewMatrix(viewport_t *v){
	Matrix4x4_CreateRotate(&v->view, v->angle[2], 0.0f, 0.0f, 1.0f);
	Matrix4x4_ConcatRotate(&v->view, v->angle[0], 1.0f, 0.0f, 0.0f);
	Matrix4x4_ConcatRotate(&v->view, v->angle[1], 0.0f, 1.0f, 0.0f);
	//recalc the v_ vectors
	//TODO simd
	//TODO shouldn't this be generated from the CAM matrix?
	Matrix4x4_Transform(&v->view, stockv_forward, v->v_forward);
	Matrix4x4_Transform(&v->view, stockv_up, v->v_up);
	Matrix4x4_Transform(&v->view, stockv_right, v->v_right);
	Matrix4x4_ConcatTranslate(&v->view, -v->pos[0], -v->pos[1], -v->pos[2]);
}

//const inline void recalcProjectionMatrix(viewport_t *v){
void recalcProjectionMatrix(viewport_t *v){
	double sine, cotangent, deltaZ;
	double radians = v->fov / 2.0 * M_PI / 180.0;
	deltaZ = v->far - v->near;
	sine = sin(radians);
	if((deltaZ == 0) || (sine == 0) || (v->aspect == 0)){
		return; //we got some nasty zeros
	}
	cotangent = cos(radians) / sine;
	v->projection.m[0][0] = cotangent/v->aspect;
	v->projection.m[1][1] = cotangent;
	v->projection.m[2][2] = -(v->far + v->near) / deltaZ;
	v->projection.m[2][3] = -1.0;
	v->projection.m[3][2] = -2.0 * v->near * v->far / deltaZ;
	v->projection.m[3][3] = 0.0;
}

//TODO figure out a faster way of doing this
//requires the cam matrix to be up to date
//requires the projection matrix to be up to date

//IDEAS FOR FASTERING
//pull things directly out of viewproj
//const inline void recalcFrustumBBoxP(viewport_t *v){
void recalcFrustumBBoxP(viewport_t *v){
	float cotangent, cotaspect;
	cotangent = v->projection.m[1][1] /2;
	cotaspect = v->projection.m[0][0] /2;
	float near = v->near;
	float far = v->far;
	float nh = near * cotangent;
	float nw = near * cotaspect;
	float fh = far * cotangent;
	float fw = far * cotaspect;

	//generate something the shape and size of the frustum
	vec_t prebboxp[24];
	prebboxp[0] = -nw;
	prebboxp[1] = -nh;
	prebboxp[2] = near;
	prebboxp[3] = nw;
	prebboxp[4] = -nh;
	prebboxp[5] = near;
	prebboxp[6] = nw;
	prebboxp[7] = nh;
	prebboxp[8] = near;
	prebboxp[9] = -nw;
	prebboxp[10] = nh;
	prebboxp[11] = near;

	prebboxp[12] = -fw;
	prebboxp[13] = -fh;
	prebboxp[14] = far;
	prebboxp[15] = fw;
	prebboxp[16] = -fh;
	prebboxp[17] = far;
	prebboxp[18] = fw;
	prebboxp[19] = fh;
	prebboxp[20] = far;
	prebboxp[21] = -fw;
	prebboxp[22] = fh;
	prebboxp[23] = far;


	//this is the "slow" part
	//now move/rotate this shape into position
	int i;
	for(i = 0; i < 8; i++){
		Matrix4x4_Transform(&v->cam, &prebboxp[i*3], &v->bboxp[i*3]);
	}

}


//requires the viewproj to already be calculated
//const inline void recalcFrustum(viewport_t *v){
void recalcFrustum(viewport_t *v){
	vec_t m[16];
	//todo cache this in the structure
	Matrix4x4_ToArrayFloatGL(&v->viewproj, m);
	//todo change this to use the actual matrix stuff
	v->frustum[0].norm[0] =	m[3] - m[0];
	v->frustum[0].norm[1] =	m[7] - m[4];
	v->frustum[0].norm[2] =	m[11]- m[8];
	v->frustum[0].d = 	m[15]- m[12];

	v->frustum[1].norm[0] =	m[3] + m[0];
	v->frustum[1].norm[1] =	m[7] + m[4];
	v->frustum[1].norm[2] =	m[11]+ m[8];
	v->frustum[1].d = 	m[15]+ m[12];

	v->frustum[2].norm[0] =	m[3] - m[1];
	v->frustum[2].norm[1] =	m[7] - m[5];
	v->frustum[2].norm[2] =	m[11]- m[9];
	v->frustum[2].d = 	m[15]- m[13];

	v->frustum[3].norm[0] =	m[3] + m[1];
	v->frustum[3].norm[1] =	m[7] + m[5];
	v->frustum[3].norm[2] =	m[11]+ m[9];
	v->frustum[3].d = 	m[15]+ m[13];

	v->frustum[4].norm[0] =	m[3] - m[2];
	v->frustum[4].norm[1] =	m[7] - m[6];
	v->frustum[4].norm[2] =	m[11]- m[10];
	v->frustum[4].d = 	m[15]- m[14];

	v->frustum[5].norm[0] =	m[3] + m[2];
	v->frustum[5].norm[1] =	m[7] + m[6];
	v->frustum[5].norm[2] =	m[11]+ m[10];
	v->frustum[5].d = 	m[15]+ m[14];

}



int viewport_recalc(viewport_t *v){
	int changed = v->changed;
	//view has changed?
	if(changed & 1){
		recalcViewMatrix(v);
		Matrix4x4_CreateFromQuakeEntity(&v->cam, v->pos[0], v->pos[1], v->pos[2], v->angle[2], v->angle[1], v->angle[0], 1.0);
		//todo figure out if something can be made faster by using a quick invert
	}
	//projection has changed?
	if(changed & 2){
		recalcProjectionMatrix(v);
	}
	//either have changed, gotta fix the combined things
	if(changed){
		Matrix4x4_Concat(&v->viewproj, &v->projection, &v->view);
		Matrix4x4_Invert_Full(&v->viewprojinv, &v->viewproj);
		recalcFrustum(v);
		recalcFrustumBBoxP(v);
	}
	v->changed = 0;
	return changed;
}


//todo make this a generic dist from plane and then inline/macro it
vec_t distPointFromNear(viewport_t *v, vec_t *p){
	vec_t *n = v->frustum[5].norm;
	vec_t dist = vec3dot(n, p) + v->frustum[5].d;
	return dist;
}


//todo a seperate view frustum class? idk
int testPointInFrustum(viewport_t *v, vec_t *p){
	int i;
	vec_t *n;
	for(i = 0; i < 6; i++){
		n = v->frustum[i].norm;
		float dist = vec3dot(n,p) + v->frustum[i].d;
		if(dist < 0.0) return FALSE;				//behind plane
	}
	return TRUE;
}
//returns 2 if the only one it fails is the nearplane
int testPointInFrustumNP(viewport_t *v, vec_t *p){
	int i;
	vec_t *n;
	float dist;
	for(i = 0; i < 5; i++){
		n = v->frustum[i].norm;
		dist = vec3dot(n,p) + v->frustum[i].d;
		if(dist < 0.0) return FALSE;				//behind plane
	}
	n = v->frustum[5].norm;
	dist = vec3dot(n,p) + v->frustum[5].d;
	if(dist < 0.0) return 2;					//behind plane
	return TRUE;
}


int testSphereInFrustum(viewport_t *v, vec_t *p, float size){
	int i;
	vec_t *n;
	float nsize = -size;
	for(i = 0; i < 6; i++){
		n = v->frustum[i].norm;
		float dist = vec3dot(n,p) + v->frustum[i].d;
		if(dist < nsize) return FALSE;				//behind plane
	}
	return TRUE;
}
//returns 2 if the only one it fails is the nearplane
int testSphereInFrustumNP(viewport_t *v, vec_t *p, float size){
	int i;
	vec_t *n;
	float dist;
	float nsize = -size;
	for(i = 0; i < 5; i++){
		n = v->frustum[i].norm;
		dist = vec3dot(n,p) + v->frustum[i].d;
		if(dist < nsize) return FALSE;				//behind plane
	}
	n = v->frustum[5].norm;
	dist = vec3dot(n,p) + v->frustum[5].d;
	if(dist < nsize) return 2;					//behind plane

	return TRUE;
}


//todo check the IQ method of doing this
int testBBoxPInFrustum(viewport_t *v, vec_t *points){
	int i;
	vec_t *n;
	float d;
	for(i = 0; i < 6; i++){
		n = v->frustum[i].norm;
		d = v->frustum[i].d;
		int j;
		for(j = 0; j < 8; j++){
			vec_t *p = &points[j*3];
			float dist = vec3dot(n,p) + d;
			if(dist > 0.0) break;		//point in front of plane.
		}
		if(j ==8) return FALSE;			//this plane didnt have any points in front of it.
	}
	return TRUE;					//all of the planes had at least 1 point in front of it
}

//TODO nearplane versions of these two guys


//returns 2 if the entire BBOXP fits within the frustum
int testBBoxPInFrustumCheckWhole(viewport_t *v, vec_t *points){
	int i;
	vec_t *n;
	float d;
	int mode = 0;
	for(i = 0; i < 6; i++){
		n = v->frustum[i].norm;
		d = v->frustum[i].d;
		int j;
		if(mode){
			for(j = 0; j < 8; j++){
				vec_t *p = &points[j*3];
				float dist = vec3dot(n,p) + d;
				if(dist > 0.0) break;		//point in front of plane.
			}
			if(j ==8) return FALSE;			//this plane didnt have any points in front of it.
		} else {
			int correct = 0;
			for(j = 0; j < 8; j++){
				vec_t *p = &points[j*3];
				float dist = vec3dot(n,p) + d;
				if(dist > 0.0) correct++;		//point behind plane
			}
			if(!correct) return FALSE;			//all points behind plane
			if(correct !=8) mode = 1;			//at least one point failed. Give up trying to check whole
		}
	}
	if(mode) return TRUE;					//all of the planes had at least 1 point in front of it
	return 2;						//all planes had ALL the points in front of it
}


int viewport_unload(viewport_t *v){
	//TODO
	return FALSE;
}




IDLIST_CODE_NOHASH(viewport, viewport_t);
