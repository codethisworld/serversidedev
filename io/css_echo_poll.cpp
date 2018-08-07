#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>//bind socket
#include <netinet/in.h>//struct sockaddr_in
#include <arpa/inet.h>//htons
#include <stdarg.h>//va_list
#include <string.h>//strerror
#include <errno.h>//errno
//#include <sys/select.h>//select
#include <poll.h>
//#include <sys/stropts.h>//INFTIM
#include <climits>//INT_MAX
#include <sys/time.h>//timeval
#include <unistd.h>//write close
#include "lhf.h"
#include "socketio.h"
/////////////////////////////////////////main//////////////////////////////////////////
int main(int argc,const char* argv[]){
	int svrfd;
	int re;
	if(argc<2){
		print_error(1,"parse input","port is needed");
	}
	int port=atoi(argv[1]);
	struct pollfd clients[MAXFDSIZE];//client fd array
	int maxi;//max index of client fd array,used for test result loop
	if(socket_bind_listen(svrfd,port)){
		print_error(1,"socket_bind_listen","");
	}

	for(int i=0;i<MAXFDSIZE;i++){
		clients[i].fd=-1;
	}
	clients[0].fd=svrfd;
	clients[0].events=POLLRDNORM;
	maxi=0;

	while(1){
		if((re=poll(clients,maxi+1,-1))<0){
			print_error(1,"select",strerror(errno));
		}else if(re==0){
			continue;
		}
		int fdnum=re;
		if(clients[0].revents&(POLLRDNORM)){
			if((re=accept(svrfd,NULL,NULL))<0){
				print_error(1,"accept",strerror(errno));
			}
			int clifd=re;
			int i;
			for(i=0;i<MAXFDSIZE;i++){
				if(clients[i].fd==-1){
					clients[i].fd=clifd;
					clients[i].events=POLLRDNORM;
					break;
				}
			}
			if(i==MAXFDSIZE){
				printf("only %d connect allowed,to much connections\n",MAXFDSIZE);
			}else if(i>maxi){
				maxi=i;
			}
			if(--fdnum<=0){
				continue;
			}
		}
		for(int i=0;i<=maxi;i++){
			char buff[MAXLINE];
			if(clients[i].revents&(POLLRDNORM|POLLERR)){
				if((re=read(clients[i].fd,buff,MAXLINE))<0){
					print_error(1,"read",strerror(errno));
				}
				if(re==0){
					printf("connect %d closed\n",clients[i].fd);
					close(clients[i].fd);
					clients[i].fd=-1;
					continue;
					//if we decrease maxi here,then some fd will not be checked.we should do it after all fds are checked
				}
				int len=re;
				if((re=write(clients[i].fd,buff,len))<0){
					print_error(1,"write",strerror(errno));
				}
				if(--fdnum<=0){
					break;
				}
			}
		}
		for(int i=maxi;i>=0 && clients[i].fd==-1;i--){
			maxi--;
		}
	}
}
