#include "globaldefs.h"
#include "naviclient.h"


#define PORT 1337

#define HOST "harry laptop"

#include <sys/types.h>
#include <sys/socket.h>

#include <stdio.h>

#include <string.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <fcntl.h>




#include "proto/requestHeader.pb-c.h"



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




void * tmpbufferdata = 0;
size_t tmpbuffersize = 0;

int naviclient_update(double time){
	//check if we have a connection to accept
	struct sockaddr_in client_address;
	socklen_t client_len = sizeof(client_address);
	int client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
	//if we get a new biter
	if(client_sockfd > 0){
		printf("GOt a bite!\n");
		//add it to the list
		numcom++;
		conlist = realloc(conlist, numcom * sizeof(connectionitem_t));
		memset(&conlist[numcom-1], 0, sizeof(connectionitem_t));
		conlist[numcom-1].client_sockfd = client_sockfd;
		conlist[numcom-1].client_address = client_address;
	}


	//loop through all attached clients, and check if they have data to send

	int i;
	for(i = 0; i < numcom; numcom++){
		//check if we have any data to send to this wee little goy
		size_t toread = 0;
		//he has something to say!
		for(toread = 0; ioctl(conlist[i].client_sockfd, FIONREAD, &toread) != -1 && toread > 0; toread=0){
			//check if we have a saved header length
			if(conlist[i].waitingread){
				//check if we have enough to read in
				if(toread < conlist[i].waitingread) break;
				//grab it
				if(tmpbuffersize < conlist[i].waitingread){
					tmpbuffersize = conlist[i].waitingread;
					if(tmpbufferdata) free(tmpbufferdata);
					tmpbufferdata = malloc(tmpbuffersize);
				}
				int readc = recv(conlist[i].client_sockfd, tmpbufferdata, conlist[i].waitingread, 0);
				//some sorta error?
				if(readc != conlist[i].waitingread){
					printf("NAVICLIENT/update error read issue 1\n");
					continue;
				}
					//parse it




				conlist[i].waitingread = 0; //ehhh
			} else { //read in a header
				//read in a FUCKING BYTE AT A TIME BECAUSE PROTOBUF IS FUCKING RETARDED
				//protobuf doesnt gaurentee the size of a packet... that means OUR FUCKING HEADERS COULD BE 0, 2, OR 5 BYTES
				//seriously, why the fuck did we ever use this?
				//god fucking dammit why cant we just send bytes and be done.



//				FFFF	U  U	 CC	K   k
//				F	U  U	C	K  k
//				FF	U  U	C	K k
//				F	 uu	 CC	K   k

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
