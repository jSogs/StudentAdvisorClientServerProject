#include "unp.h"

int main(int argc, char **argv) {
	int					sockfd;
	struct sockaddr_in	servaddr;

	/* Check argument count */
	if (argc != 2){
		printf("usage: tcpcli <IPaddress>");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0); //create socket

	/* Initialize server address structure */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(3023);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (SA *) &servaddr, sizeof(servaddr)); //connect socket to server

	mainMenu(stdin,sockfd);		//process the request

	exit(0);
}
