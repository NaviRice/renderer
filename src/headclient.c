#include "globaldefs.h"
#include "headclient.h"

#define PORT 4007
//#define HOST "navirice"
#define HOST "75.136.80.95"
#define HOST "navirice"
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



typedef struct packetdata_s {
	float x;
	float y;
	float z;
} packetdata_t;


viewport_t headclient_kinectvp = {0};
int headclient_generateKinectMats(void){
	headclient_kinectvp.type = 1;
	headclient_kinectvp.pos[0] = 0.0;
	headclient_kinectvp.pos[1] = 0.0;
	headclient_kinectvp.pos[2] = 0.0;
	headclient_kinectvp.angle[1] = 180;
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

int fuck = 0;
vec4_t headclient_headpos = {0.0, 0.0, 4.0, 0.0};
int headclient_update(void){
	fuck++;
	size_t toread = 0;
	int captured = 0;
	vec4_t fakehead_pos = {0.0, 0.0, 0.0, 1.0};
	ioctl(consock, FIONREAD, &toread);
//	printf("to read %i %i\n", toread, sizeof(packetdata_t));
	for(toread = 0; ioctl(consock, FIONREAD, &toread) != -1 && toread >= sizeof(packetdata_t); toread = 0){
		int read = recv(consock, &headpos, sizeof(packetdata_t), 0);
		if(read == sizeof(packetdata_t)){
			fakehead_pos[0] = headpos.x;
			fakehead_pos[1] = headpos.y;
			fakehead_pos[2] = headpos.z/1000.0;
			printf("new head pos is %f %f %f\n", fakehead_pos[0], fakehead_pos[1], fakehead_pos[2]);
			captured++;
		}
	}
//	printf("captured ios %i\n", captured);
	if(!captured) return 0;
//	fakehead_pos[0] = 1.0;
//	fakehead_pos[1] = 0.0;
//	fakehead_pos[2] = (sin(fuck * 0.002)* 0.5 + 0.5);
	//get Z back into clip space

//	vec4_t jumbo_jet = {0.0, 0.0, -fakehead_pos[2], 1.0};

//	Matrix4x4_Transform4(&headclient_kinectvp.projection, jumbo_jet, fakehead_pos);

//	fakehead_pos[2] = headclient_kinectvp.projection.m[2][2] * jumbo_jet[2] + headclient_kinectvp.viewproj.m[3][2];
//	fakehead_pos[3] = headclient_kinectvp.projection.m[2][3] * jumbo_jet[2] + headclient_kinectvp.viewproj.m[3][3];
//	fakehead_pos[2] /= fakehead_pos[3];

//	fakehead_pos[3] = fakehead_pos[2];
//	fakehead_pos[2] = headclient_kinectvp.projection.m[2][2];
//	printf("new head pos is %f %f %f\n", fakehead_pos[0], fakehead_pos[1], fakehead_pos[2]);

//	fakehead_pos[3] =
//		headclient_kinectvp.viewproj.m[3][2]/
//		(fakehead_pos[2] - (headclient_kinectvp.viewproj.m[2][2] / headclient_kinectvp.viewproj.m[2][3]));

//	fakehead_pos[0] *= fakehead_pos[3];
//	fakehead_pos[1] *= fakehead_pos[3];
//	fakehead_pos[2] *= fakehead_pos[3];


//ok finalized coolio

	fakehead_pos[3] = fakehead_pos[2];
	fakehead_pos[0] *=fakehead_pos[2];
	fakehead_pos[1] *=fakehead_pos[2];
	fakehead_pos[2] *= headclient_kinectvp.projection.m[2][2];
	Matrix4x4_Transform4(&headclient_kinectvp.viewprojinv, fakehead_pos, headclient_headpos);
	//ok, lets move it back and compare


	printf("real head pos is %f %f %f\n", headclient_headpos[0], headclient_headpos[1], headclient_headpos[2]);
	return TRUE;
}
