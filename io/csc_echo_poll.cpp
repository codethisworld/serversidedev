#include <stdio.h>//fileno
#include <stdlib.h>
#include <sys/socket.h>//socket
#include <netinet/in.h>//struct sockaddr
#include <arpa/inet.h>//htons
#include <stdarg.h>//va_list
#include <string.h>//strerror
#include <errno.h>//errno
//#include <sys/select.h>//select
#include <poll.h>
#include <limits.h>
#include <unistd.h>//read write close

#include "lhf.h"
#include "socketio.h"
//////////////////////////functions///////////////////////////////////
//////////////////////////main////////////////////////////////////////
int main(int argc,char* argv[]){
	int svrfd;
	int re;
	if(argc<2){
		print_error(1,"parse input","port is needed");
	}
	int port=atoi(argv[1]);
	int stdinfd=fileno(stdin);
	int stdoutfd=fileno(stdout);
	if(socket_connect(svrfd,port)){
		print_fail("debug");
	}
	
	int stdineof=0;
	int svrfdeof=0;
	struct pollfd fds[2];
	fds[0].fd=stdinfd;
	fds[0].events=POLLRDNORM;
	fds[1].fd=svrfd;
	fds[1].events=POLLRDNORM;
	while(1){
		if((re=poll(fds,2,-1))<0){
			print_error(1,"select",strerror(errno));
		}
		if(re==0){
			continue;	
		}
		if(!stdineof && (fds[0].revents&(POLLRDNORM|POLLERR))){
			//printf("stdin ready\n");
			read_write(stdinfd,svrfd,stdineof);
		}
		if(!svrfdeof && (fds[1].revents&(POLLRDNORM|POLLERR))){
			//printf("svrfd ready\n");
			read_write(svrfd,stdoutfd,svrfdeof);
			if(svrfdeof){
				exit(0);
			}
		}
	}
}
