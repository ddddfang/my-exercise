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
		if(sockfd < 0){
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
		if(bind(sockfd,p->ai_addr,p->ai_addrlen) < 0){
			fprintf(stderr,"bind error %d\n",sockfd);
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
	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	FD_ZERO(&master);    // clear the master and temp sets
	FD_ZERO(&read_fds);
	FD_SET(sockfd, &master);    // add the listener to the master set
	int fdmax = sockfd;// so far, it's this one
	while(1){
		read_fds = master; // copy it
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}
		for(int i=0;i<=fdmax;i++){
			if(FD_ISSET(i, &read_fds)){	// we got one!
				if(i == sockfd){
					//setup a new connection
					struct sockaddr_storage their_addr;
					socklen_t sin_size = sizeof(their_addr);
					int sock_new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);	//accept may cause block.
					if(sock_new_fd == -1){
						fprintf(stderr,"accept error.\n");
						continue;
					} else {
						FD_SET(sock_new_fd, &master);    // add the sock_new_fd to the master set
						if(sock_new_fd > fdmax){	//update fdmax
							fdmax = sock_new_fd;
						}
						char remoteIP[INET6_ADDRSTRLEN];
						inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),remoteIP,sizeof(remoteIP));
						printf("server(%s): got connection from %s\n",ipstr,remoteIP);

					}
				} else {//handle data from a client
					char buf[256];    // buffer for client data
					int nbytes;
					if((nbytes = recv(i,buf,sizeof(buf),0)) <= 0){// got error or connection closed by client
						if (nbytes == 0) {// connection closed
							printf("selectserver: socket %d hung up\n", i);
						} else {
							perror("recv");
						}
						close(i); // bye!
						FD_CLR(i, &master); // remove from master set
					} else {// we got some data from a client
						for(int j = 0; j <= fdmax; j++) {// send to everyone!
							if (FD_ISSET(j, &master)) {
								if (j != sockfd && j != i) {// except the listener and ourselves
									if (send(j, buf, nbytes, 0) == -1) {
										perror("send");
									}
								}
							}
						}
					}
				}
			}
		}
#if 0
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
#endif
	}
	return 0;
}


