#include "globaldefs.h"
#include "headclient.h"

#define PORT 4007
#define HOST "75.136.80.95"
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
	headclient_kinectvp.angle[0] = 180;
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
	if(connect(consock, (struct sockaddr*)&servaddr, sizeof(servaddr)) <0){
		printf("failed to connect\n");
		return 0;
	}
	int flags = fcntl(consock, F_GETFL, 0);
	fcntl(consock, F_SETFL, flags | O_NONBLOCK);
	printf("olo\n");
	return TRUE;
}
packetdata_t headpos = {0};

vec3_t headclient_headpos = {0};
int headclient_update(void){
	size_t toread = 0;
	int captured = 0;
	vec3_t fakehead_pos = {0.0, 0.0, 0.0};
	ioctl(consock, FIONREAD, &toread);
//	printf("to read %i %i\n", toread, sizeof(packetdata_t));
	for(toread = 0; ioctl(consock, FIONREAD, &toread) != -1 && toread >= sizeof(packetdata_t); toread = 0){
		int read = recv(consock, &headpos, sizeof(packetdata_t), 0);
		if(read == sizeof(packetdata_t)){
			printf("new head pos is %f %f %f\n", headpos.x, headpos.y, headpos.z/1000.0);
			fakehead_pos[0] = headpos.x;
			fakehead_pos[1] = headpos.y;
			fakehead_pos[2] = headpos.z/1000.0;
			captured++;
		}
	}
//	printf("captured ios %i\n", captured);
	if(!captured) return 0;
	Matrix4x4_Transform(&headclient_kinectvp.viewprojinv, fakehead_pos, headclient_headpos);
//	headclient_headpos[0] /= headclient_headpos[3];
//	headclient_headpos[1] /= headclient_headpos[3];
	//ok now we get real head location from the input
	printf("real head pos is %f %f %f\n", headclient_headpos[0], headclient_headpos[1], headclient_headpos[2]);
	return TRUE;
}
