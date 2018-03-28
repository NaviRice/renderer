#include "globaldefs.h"
#include "naviclient.h"


#define PORT 1337


#include <sys/types.h>
#include <sys/socket.h>

#include <stdio.h>

#include <string.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <fcntl.h>




//ok math time!

#include <math.h>
#include "mathlib.h"
#include "matrixlib.h"
/*
def llarToWorld(lat, lon, alt, rad):
    # see: http://www.mathworks.de/help/toolbox/aeroblks/llatoecefposition.html
    f  = 0                              # flattening
    ls = atan((1 - f)**2 * tan(lat))    # lambda

    x = rad * cos(ls) * cos(lon) + alt * cos(lat) * cos(lon)
    y = rad * cos(ls) * sin(lon) + alt * cos(lat) * sin(lon)
    z = rad * sin(ls) + alt * sin(lat)

    return c4d.Vector(x, y, z)
*/

//for starters lets assume alt is 0
void calcWORLDpos(double lon, double lat, double alt, double * x, double * y, double * z){
	double deg2rad = 180.0/M_PI;
	double rad = 6378137.0;
	double f = 1.0/298.257223563;
	double coslon = cosl(lon*deg2rad);
	double sinlon = sinl(lon*deg2rad);
	double coslat = cosl(lat*deg2rad);
	double sinlat = sinl(lat*deg2rad);

	double ff = (1.0-f); ff*=ff;
	double c = 1.0/sqrtl(coslat*coslat + ff*sinlat*sinlat);
	double s = c * ff;
	*x = (rad * c + alt)* coslat * coslon;
	*y = (rad * c + alt)* coslat * sinlon;
	*z = (rad * s + alt)* sinlat;
}

void calcWORLDpossimple(double lon, double lat, double alt, double *x, double *y, double *z){
	double deg2rad = 180.0/M_PI;
	double rad = 6378137.0;
	double coslon = cosl(lon*deg2rad);
	double sinlon = sinl(lon*deg2rad);
	double coslat = cosl(lat*deg2rad);
	double sinlat = sinl(lat*deg2rad);


	*x = coslat * coslon * (rad + alt);
	*y = coslat * sinlon * (rad + alt);
	*z = sinlat * (rad + alt);
}



//used https://stackoverflow.com/questions/10473852/convert-latitude-and-longitude-to-point-in-3d-space as a ref
//general idea

//calculate 3d doubles from lat/long (and i mean doubles) of everything
//store waypoints, etc, as those 3d doubles

//set origin to current position
//(subtract shit from it)

//rotate everything such that north? is +z, up is +y
//further rotate such that
//later rotate everything such that car forward is +z?



//currently only one waypoint

//todo put these in structs
double waypointlon;
double waypointlat;
double waypointx;
double waypointy;
double waypointz;
//and then final stuff here
vec3_t waypointfinalnorot;
vec3_t waypointfinal;

double currentposlon;
double currentposlat;
double currentposx;
double currentposy;
double currentposz;

matrix4x4_t worldrotty;


int waypointneedsupdate = 1;
int currentposneedsupdate = 1;


void updateWayPoint(double lat, double lon){
	waypointlat = lat;
	waypointlon = lon;
	waypointneedsupdate = 1;
}

void recalcNavLocs(void){
	if(currentposneedsupdate){
		calcWORLDpos(currentposlat, currentposlon, 0.0, &currentposx, &currentposy, &currentposz);
		//todo recalc rotation matrix
		//todo double check this shit
//		Matrix4x4_CreateRotate(&worldrotty, currentposlat, 1.0, 0.0, 0.0);
//		Matrix4x4_ConcatRotate(&worldrotty, currentposlon, 0.0, 1.0, 0.0);
		Matrix4x4_CreateRotate(&worldrotty, currentposlon, -1.0, 0.0, 0.0);
		Matrix4x4_ConcatRotate(&worldrotty, currentposlat, 0.0, -1.0, 0.0);

		//should end up with +z as north, +y as "up", and +x as east
	}
	if(waypointneedsupdate){
		calcWORLDpos(currentposlat, currentposlon, 0.0, &waypointx, &waypointy, &waypointz);
	}
	if(currentposneedsupdate || waypointneedsupdate){
		//find delta
		waypointfinalnorot[0] = waypointx - currentposx;
		waypointfinalnorot[1] = waypointy - currentposy;
		waypointfinalnorot[2] = waypointz - currentposz;
		//now rotate
		Matrix4x4_Transform(&worldrotty, waypointfinalnorot, waypointfinal);
	}
	waypointneedsupdate = currentposneedsupdate = 0;
}


void updateCurrentPos(double lat, double lon){
	currentposlat = lat;
	currentposlon = lon;
	currentposneedsupdate = 1;
}





#include "proto/requestHeader.pb-c.h"
#include "proto/step.pb-c.h"
#include "proto/response.pb-c.h"
#include "proto/location.pb-c.h"
#include "proto/rotationRate.pb-c.h"
#include "proto/accelerationForce.pb-c.h"



typedef struct connectionitem_s {
	struct sockaddr_in client_address;
	int client_sockfd;
	size_t waitingread;
	int waitingreadtype;

	void * tdata;
	size_t tdatasize;
	size_t tdataplace;
} connectionitem_t;

connectionitem_t * conlist;
size_t numcom = 0; // just gonna fudge it



int server_sockfd;
int naviclient_init(void){

	int server_len;
	struct sockaddr_in server_address;
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT);
	server_len = sizeof(server_address);


	int reuseon = 1;



	if(setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseon, sizeof(reuseon)) < 0){
		printf("NAVICLIENT/init unable to set socket reusable\n");
		close(server_sockfd);
		return FALSE;
	}

	if(ioctl(server_sockfd, FIONBIO, (char *)&reuseon) < 0){
		printf("NAVICLIENT/init unable to set socket nonblocking\n");
		close(server_sockfd);
		return FALSE;
	}

	int flags = fcntl(server_sockfd, F_GETFL, 0);
	if(fcntl(server_sockfd, F_SETFL, flags | O_NONBLOCK) < 0){
		printf("NAVICLIENT/init unable to set socket nonblocking 2\n");
		close(server_sockfd);
		return FALSE;
	}


	if(bind(server_sockfd, (struct sockaddr *)&server_address, server_len)){
		printf("NAVICLIENT/init unable to bind\n");
		close(server_sockfd);
		return FALSE;
	}

	if(listen(server_sockfd, 5)){
		printf("NAVICLIENT/init unable to listen\n");
		close(server_sockfd);
		return FALSE;
	}
	printf("listen successsfullll\n");

	return TRUE;
}



int parseItThePacket(void * data, size_t datasize, int type){
	Navirice__Proto__Step *st;
	Navirice__Proto__Location *loc;
	Navirice__Proto__AccelerationForce *acc;
	Navirice__Proto__RotationRate *gyr;
	switch(type){
		case 0: //step
			st = navirice__proto__step__unpack(NULL, datasize, data);
			if(!st){
				printf("Fuck protobuff... Seriously!\n");
				return 1;
			}
			printf("Got a fucking step %f %f %s %s\n", st->latitude, st->longitude, st->description, st->icon);
			updateWayPoint(st->latitude, st->longitude);
			navirice__proto__step__free_unpacked(st, NULL);
		break;
		case 1: //current loc
			loc = navirice__proto__location__unpack(NULL, datasize, data);
			if(!loc){
				printf("Fuck protobuff... Seriously!\n");
				return 1;
			}
			printf("Got a fucking location %f %f\n", loc->latitude, loc->longitude);
			updateCurrentPos(loc->latitude, loc->longitude);
			navirice__proto__location__free_unpacked(loc, NULL);
		break;
		case 2: //accel
			acc = navirice__proto__acceleration_force__unpack(NULL, datasize, data);
			if(!acc){
				printf("Fuck protobuff... Seriously!\n");
				return 1;
			}
			printf("Got a fucking accel %f %f %f\n", acc->x, acc->y, acc->z);
			navirice__proto__acceleration_force__free_unpacked(acc, NULL);
		break;
		case 3: //gyro
			gyr = navirice__proto__rotation_rate__unpack(NULL, datasize, data);
			if(!gyr){
				printf("Fuck protobuff... Seriously!\n");
				return 1;
			}
			printf("Got a fucking gyro %f %f %f\n", gyr->x, gyr->y, gyr->z);
			navirice__proto__rotation_rate__free_unpacked(gyr, NULL);
		break;
	}
	return 0;
}

int naviclient_update(double time){
	//check if we have a connection to accept
	struct sockaddr_in client_address;
	socklen_t client_len = sizeof(client_address);
	int client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
	//if we get a new biter
	if(client_sockfd > 0){
		printf("Got a bite!\n");
		//add it to the list
		numcom++;
		conlist = realloc(conlist, numcom * sizeof(connectionitem_t));
		memset(&conlist[numcom-1], 0, sizeof(connectionitem_t));
		conlist[numcom-1].client_sockfd = client_sockfd;
		conlist[numcom-1].client_address = client_address;
	}

	//TODO figure out how to handle a client disconnecting


	//loop through all attached clients, and check if they have data to send

	int i;
	for(i = 0; i < numcom; i++){
		//check if we have any data to send to this wee little goy
		size_t toread = 0;
		//he has something to say!
		for(toread = 0; ioctl(conlist[i].client_sockfd, FIONREAD, &toread) != -1 && toread > 0; toread=0){
			//check if we have a saved header length
			if(conlist[i].waitingread){
				//check if we have enough to read in
				if(toread < conlist[i].waitingread) break;
				//first check if i gotta resize the buf tho
				if(conlist[i].waitingread > conlist[i].tdatasize){
					conlist[i].tdatasize = conlist[i].waitingread;
					if(conlist[i].tdata) free(conlist[i].tdata);
					conlist[i].tdata = malloc(conlist[i].tdatasize);
				}

				int readc = recv(conlist[i].client_sockfd, conlist[i].tdata, conlist[i].waitingread, 0);
				//some sorta error?
				if(readc != conlist[i].waitingread){
					printf("NAVICLIENT/update error read issue 1\n");
					continue;
				}
				//parse it

				Navirice__Proto__Response resp = NAVIRICE__PROTO__RESPONSE__INIT;
				resp.status = parseItThePacket(conlist[i].tdata, conlist[i].waitingread, conlist[i].waitingreadtype);
				size_t sizzle = navirice__proto__response__get_packed_size(&resp);
				if(sizzle > conlist[i].tdatasize){
					conlist[i].tdatasize = sizzle;
					if(conlist[i].tdata) free(conlist[i].tdata);
					conlist[i].tdata = malloc(conlist[i].tdatasize);
				}
				navirice__proto__response__pack(&resp, conlist[i].tdata);
				send(conlist[i].client_sockfd, conlist[i].tdata, sizzle, 0);





				conlist[i].waitingread = 0; //ehhh
			} else { //read in a header
				//read in a FUCKING BYTE AT A TIME BECAUSE PROTOBUF IS FUCKING RETARDED
				//protobuf doesnt gaurentee the size of a packet... that means OUR FUCKING HEADERS COULD BE 0, 2, OR 5 BYTES
				//seriously, why the fuck did we ever use this?
				//god fucking dammit why cant we just send bytes and be done.



//				FFFF	U  U	 CC	K   K
//				F	U  U	C	K  K
//				FF	U  U	C	K K
//				F	 uu	 CC	K   K

				//read in one byte
				//first check if i gotta resize the buf tho
				if(conlist[i].tdataplace >= conlist[i].tdatasize){
					conlist[i].tdatasize = conlist[i].tdataplace+1;
					conlist[i].tdata = realloc(conlist[i].tdata, conlist[i].tdatasize);
				}
				//read that faggot in
				int readc = recv(conlist[i].client_sockfd, conlist[i].tdata + conlist[i].tdataplace, 1, 0);
				//some sorta error?
				if(readc != 1){
					printf("NAVICLIENT/update error read issue 2\n");
					continue;
				}
				conlist[i].tdataplace++;
				//attempt to parse it
				Navirice__Proto__RequestHeader * readhead = navirice__proto__request_header__unpack(NULL, conlist[i].tdataplace, conlist[i].tdata);
				if(!readhead){ //didnt read enough
					continue;
				}
				//read enough
				conlist[i].waitingread = readhead->length;
				conlist[i].waitingreadtype = readhead->type;
				conlist[i].tdataplace = 0;
				navirice__proto__request_header__free_unpacked(readhead, NULL);

			}
		}

	}

	return TRUE;
}
