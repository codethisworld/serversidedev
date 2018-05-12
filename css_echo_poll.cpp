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
/////////////////////////////////////////variables/////////////////////////////////////
enum {MAXFDSIZE=500,BACKLOG=1000,MAXLINE=1024};
/////////////////////////////////////////func//////////////////////////////////////////
#define print_error(A,B,C) print_error_with_pos(__FILE__,__LINE__,__func__,A,B,C)
void printf_and_exit(int exitstatus,const char* format,...){
	va_list va;
	va_start(va,format);
	vprintf(format,va);
	va_end(va);
	exit(exitstatus);
}
inline void print_error_with_pos(const char* file,int line,const char* func,int exitstatus,const char* who,const char* why){
	printf_and_exit(exitstatus,"%s:%d::%s\t%s failed for:%s\n",file,line,func,who,why);
}
void socket_bind_listen(int& svrfd,const int port){
	int re;
	struct sockaddr_in svraddr;
	bzero(&svraddr,sizeof(svraddr));
	if((svrfd=socket(AF_INET,SOCK_STREAM,0))<0){
		print_error(1,"socket",strerror(errno));
	}
	svraddr.sin_family=AF_INET;
	svraddr.sin_port=htons(port);
	svraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	if((re=bind(svrfd,(struct sockaddr*)(&svraddr),sizeof(svraddr)))<0){
		print_error(1,"bind",strerror(errno));
	}
	if((re=listen(svrfd,BACKLOG))<0){
		print_error(1,"listen",strerror(errno));
	}
}
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
	socket_bind_listen(svrfd,port);

	for(int i=0;i<MAXFDSIZE;i++){
		clients[i].fd=-1;
	}
	clients[0].fd=svrfd;
	clients[0].events=POLLRDNORM;
	maxi=0;

	while(1){
		if((re=poll(clients,maxi+1,INT_MAX))<0){
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
