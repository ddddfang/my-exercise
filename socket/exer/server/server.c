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
#define BACKLOG	10


void sigchld_handler(int s)
{
	int saved_errno = errno;
	while(waitpid(-1,NULL,WNOHANG) > 0);
	errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa)
{
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	//get address info
	int status;
	struct addrinfo hints;
	struct addrinfo *p, *serinfo;	//pointer to a list
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;		//don't care ipv4 or v6
	hints.ai_socktype = SOCK_STREAM;	//tcp stream sockets
	hints.ai_flags = AI_PASSIVE;		//tell the program to bind to the ip of host
	if((status = getaddrinfo(NULL, PORT, &hints, &serinfo)) != 0){
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
		int yes=1;
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
			fprintf(stderr,"setsockopt error %d\n",sockfd);
			return -1;
		}
		//once you have a socket, you may have to associate it with a port on your local machine.
		//port number is used by the kernel to match an incoming packet to a certain process's socket descriptor.
		//if you only care about remote port number, this step can be ignored. connect() will do this internal.
		if(bind(sockfd,p->ai_addr,p->ai_addrlen) == -1){
			fprintf(stderr,"bind error %d\n",sockfd);
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
	freeaddrinfo(serinfo);	//useless now.

	if(listen(sockfd, BACKLOG) == -1){
		fprintf(stderr,"listen error.\n");
		return -1;
	}

	struct sigaction sa;
	sa.sa_handler = sigchld_handler;	//reap all dead process. zombie child process
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa, NULL) == -1){
		fprintf(stderr,"sigaction error.\n");
		return -1;
	}

	printf("server(%s): waiting for connections...\n",ipstr);
	while(1){
		struct sockaddr_storage their_addr;
		socklen_t sin_size = sizeof(their_addr);
		int sock_new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if(sock_new_fd == -1){
			fprintf(stderr,"accept error.\n");
			continue;
		}
		char s[INET6_ADDRSTRLEN];
		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s,sizeof(s));
		printf("server(%s): got connection from %s\n",ipstr,s);

		if(!fork()){
			//child process.
			close(sockfd);	//child does not need listener, focus this single thing is enough !

			char sent_str[] = "Hello, emmmm...\n";
			if(send(sock_new_fd, sent_str, sizeof(sent_str), 0) == -1){
				printf("send error\n");
			}
			exit(0);
		}
		close(sock_new_fd);	//parent does not need send()/recv(), so close is just OK.
	}
	return 0;
}


