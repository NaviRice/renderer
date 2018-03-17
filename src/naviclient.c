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



typedef struct connectionitem_s {
	struct sockaddr_in client_address;
	int client_sock;
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

	return TRUE;
}


int naviclient_update(double time){
	//check if we have a connection to accept
	


	//loop through all attached clients, and check if they have data to send

	int i;
	for(i = 0; i < numcom; numcom++){
	}

	return TRUE;
}
