#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
int getaddrinfo(	const char *node,		//eg. "www.baidu.com" or IP.
					const char *service,	//wg. "http" or port-number(in /etc/services)
					const struct addrinfo *hints,	//addrinfo you have already filled out with relevant information
					struct addrinfo **res);	//result, a pointer to a linked-list
*/
int main()
{
	printf("hello socket exer.\n");
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;	//pointer to a list

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;		//don't care ipv4 or v6
	hints.ai_socktype = SOCK_STREAM;	//tcp stream sockets
	hints.ai_flags = AI_PASSIVE;		//fill in my ip for me

	if((status = getaddrinfo(NULL,"3490",&hints,&servinfo)) != 0){
		fprintf(stderr,"getaddrinfo error %s\n",gai_strerror(status));
		exit(1);
	}
	freeaddrinfo(servinfo);
	return 0;
}


