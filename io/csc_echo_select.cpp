#include <stdio.h>//fileno
#include <stdlib.h>
#include <sys/socket.h>//socket
#include <netinet/in.h>//struct sockaddr
#include <arpa/inet.h>//htons
#include <stdarg.h>//va_list
#include <string.h>//strerror
#include <errno.h>//errno
#include <sys/select.h>//select
#include <unistd.h>//read write close

#include "lhf.h"
#include "socketio.h"
//////////////////////////variables///////////////////////////////////
//////////////////////////functions///////////////////////////////////
//////////////////////////main////////////////////////////////////////
int main(int argc,char* argv[]){
	int svrfd;
	int re;
	int stdinfd=fileno(stdin);
	int stdoutfd=fileno(stdout);
	fd_set rset,allset; 
	if(socket_connect(svrfd,25000)){
		print_fail("debug");
	}
	
	FD_ZERO(&allset);
	FD_SET(stdinfd,&allset);
	FD_SET(svrfd,&allset);
	int maxfd=svrfd;
	if(stdinfd>maxfd){
		maxfd=stdinfd;
	}
	int stdineof=0;
	int svrfdeof=0;
	while(1){
		rset=allset;
		if((re=select(maxfd+1,&rset,NULL,NULL,NULL))<0){
			print_error(1,"select",strerror(errno));
		}
		if(re==0){
			continue;	
		}
		if(!stdineof && FD_ISSET(stdinfd,&rset)){
			//printf("stdin ready\n");
			if(read_write_shutdown_if_read_eof(stdinfd,svrfd,stdineof)){
				print_error(1,"read_write_shutdown_if_read_eof","");
			}
		}
		if(!svrfdeof && FD_ISSET(svrfd,&rset)){
			//printf("svrfd ready\n");
			if(read_write_shutdown_if_read_eof(svrfd,stdoutfd,svrfdeof)){
				print_error(2,"read_write_shutdown_if_read_eof","");
			}
			if(svrfdeof){
				exit(0);
			}
		}
	}
}
