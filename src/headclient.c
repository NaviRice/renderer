#include "globaldefs.h"
#include "headclient.h"

#define PORT 4007
//#define HOST "75.136.80.95"
//#define HOST "navirice"
//#define HOST "localhost"
#define HOST "192.168.43.183"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#include "matrixlib.h"
#include "viewportmanager.h"

#include <math.h>
#include "mathlib.h"



vec3_t headclient_headpos = {0.0, 0.0, 0.0};

typedef struct packetdata_s {
	float x;
	float y;
	float z;
} packetdata_t;


viewport_t headclient_kinectvp = {0};
int headclient_generateKinectMats(void){
	headclient_kinectvp.type = 1;
	headclient_kinectvp.pos[0] = 0.00;
	headclient_kinectvp.pos[1] = 0.5;
	headclient_kinectvp.pos[2] = 1.5;
	headclient_kinectvp.angle[1] = 180;
	headclient_kinectvp.angle[1] = 200;
	headclient_kinectvp.angle[0]= -20.0;
	headclient_kinectvp.fov = 70.6;
	headclient_kinectvp.aspect=70.6/60.0;
	headclient_kinectvp.near = 0.1;
	headclient_kinectvp.far = 10.0;
	headclient_kinectvp.changed = 3;
	viewport_recalc(&headclient_kinectvp);


	return FALSE;
}

int consock = 0;
int headclient_init(void){
	headclient_generateKinectMats();
	//connect to server
	struct sockaddr_in servaddr = {0};
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, HOST, &servaddr.sin_addr);
	servaddr.sin_port = htons(PORT);
	if ((consock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("could not socket\n");
		return 0;
	}
	printf("bogus\n");
	int flags;
//	int flags = fcntl(consock, F_GETFL, 0);
//	fcntl(consock, F_SETFL, flags | O_NONBLOCK);
	if(connect(consock, (struct sockaddr*)&servaddr, sizeof(servaddr)) <0){
		printf("failed to connect\n");
		return 0;
	}
	flags = fcntl(consock, F_GETFL, 0);
	fcntl(consock, F_SETFL, flags | O_NONBLOCK);
	printf("olo\n");
	return TRUE;
}
packetdata_t headpos = {0};



typedef struct headclient_history_s {
	double time;
	vec3_t pos;
	//todo more?
} headclient_history_t;

//todo make these nice in a struct n shit
headclient_history_t headclient_poshistory[2] = {0};
headclient_history_t headclient_delta = {0};
vec3_t headclient_deltavel = {0};
int headclient_update(double time){
	if(consock < 1) return 0;
	size_t toread = 0;
	int captured = 0;
	vec3_t fakehead_pos = {0.0, 0.0, 0.0};
	ioctl(consock, FIONREAD, &toread);
//	printf("to read %i %i\n", toread, sizeof(packetdata_t));
	for(toread = 0; ioctl(consock, FIONREAD, &toread) != -1 && toread >= sizeof(packetdata_t); toread = 0){
		int read = recv(consock, &headpos, sizeof(packetdata_t), 0);
		if(read == sizeof(packetdata_t)){
			fakehead_pos[0] = headpos.x;
			fakehead_pos[1] = headpos.y;
			fakehead_pos[2] = headpos.z/1000.0;
//			printf("new head pos is %f %f %f\n", fakehead_pos[0], fakehead_pos[1], fakehead_pos[2]);
			captured++;
		}
	}
	if(!captured){
		//predict
		//get delta in seconds
		double tdelt = time - headclient_poshistory[0].time;
		//if we get too fucky, just give up
		if(tdelt >= 0.25)return 0;
		//take delta current time from prev multiply it by the vel
		vec3_t toff  = {0};
		vec3mult(toff, headclient_deltavel, tdelt);
		//add it onto the previous
		vec3addvec(headclient_headpos, headclient_poshistory[0].pos, toff);
		return 0;
	}
	//assumed else


//	if(!captured) return 0;
//note, this is distance based, not clip based

	vec4_t i1 = {fakehead_pos[0], fakehead_pos[1], 0.0, 1.0};
	vec4_t v1 = {0}; //P
	vec4_t v2 = {0}; //p

	Matrix4x4_Transform4(&headclient_kinectvp.viewprojinv, i1, v1);
	v1[0] /= v1[3]; v1[1] /= v1[3]; v1[2] /= v1[3];

	i1[2] = 1.0;
	Matrix4x4_Transform4(&headclient_kinectvp.viewprojinv, i1, v2);
	v2[0] /= v2[3]; v2[1] /= v2[3]; v2[2] /= v2[3];

	vec3subvec(v2, v2, v1);
	vec3norm2(v2, v2);

	//at this point v1 is p, and v2 is v of a ray

	//standard distance along a ray shits

	vec3mult(v2, v2, fakehead_pos[2]);
	vec3addvec(headclient_headpos, v1, v2);
	//update the history

	//TODO
	//if i get more than one head update packet, i use that both to update the history
	//so really this should be in the loop

	headclient_poshistory[1] = headclient_poshistory[0];
	headclient_poshistory[0].pos[0] = headclient_headpos[0];
	headclient_poshistory[0].pos[1] = headclient_headpos[1];
	headclient_poshistory[0].pos[2] = headclient_headpos[2];
	headclient_poshistory[0].time = time;

	headclient_delta.time = headclient_poshistory[0].time - headclient_poshistory[1].time;
	vec3subvec(headclient_delta.pos, headclient_poshistory[0].pos, headclient_poshistory[1].pos);
	//now get a velocity (per second)
	vec3div(headclient_deltavel, headclient_delta.pos, headclient_delta.time);

	printf("Got a head position update... delta time %f, delta pos %f %f %f, vel %f %f %f\n", headclient_delta.time,
		headclient_delta.pos[0],headclient_delta.pos[1],headclient_delta.pos[2],
		headclient_deltavel[0],headclient_deltavel[1],headclient_deltavel[2]);

	return TRUE;
}
