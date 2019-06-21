#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>//inet_ntop
#include <sys/wait.h>
#include <signal.h>
#include <netinet/in.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>//gethostname()
#include <errno.h>

#define PORT	"3490"
#define MAXDATASIZE	100


void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	if(argc != 2){
		fprintf(stderr,"usage: ./xxx ip \n");
		return -1;
	}

	//get address info of server.
	int status;
	struct addrinfo hints;
	struct addrinfo *p, *serinfo;	//pointer to a list
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;		//don't care ipv4 or v6
	hints.ai_socktype = SOCK_STREAM;	//tcp stream sockets
	if((status = getaddrinfo(argv[1], PORT, &hints, &serinfo)) != 0){
		fprintf(stderr,"getaddrinfo error %s\n",gai_strerror(status));
		return -1;
	}

	//look up all address info, use the first to get file descriptor and bind.
	int sockfd = 0;	//sockfd used for new connections
	for(p = serinfo; p != NULL; p = p->ai_next){
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);	//make no sense if you only get descriptor....
		if(sockfd == -1){
			fprintf(stderr,"socket error %d\n",sockfd);
			continue;
		}
		if(connect(sockfd,p->ai_addr,p->ai_addrlen) == -1){
			fprintf(stderr,"connect error %d\n",sockfd);
			close(sockfd);
			continue;
		}
		break;
	}

	if(p == NULL){	//dont find a valid address.
		fprintf(stderr,"p == NULL\n");
		return -1;
	}

	char ipstr[INET6_ADDRSTRLEN];
	memset(ipstr, 0, sizeof(ipstr));
	inet_ntop(p->ai_family,get_in_addr((struct sockaddr *)(p->ai_addr)),ipstr,sizeof(ipstr));//convert ip addr to string
	printf("client: connecting to %s\n",ipstr);
	freeaddrinfo(serinfo);

	char buf[MAXDATASIZE];
	int numbytes = 0;
	if((numbytes = recv(sockfd,buf,MAXDATASIZE-1,0)) == -1){
		fprintf(stderr,"recv error.\n");
		return -1;
	}
	buf[numbytes] = '\0';
	printf("client: received %s\n",buf);
	close(sockfd);
	return 0;
}


