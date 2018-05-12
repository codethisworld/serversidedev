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
//////////////////////////variables///////////////////////////////////
enum {MAXFDSIZE=500,MAXLINE=2};
//////////////////////////functions///////////////////////////////////
#define print_error(A,B,C) print_error_with_pos(__FILE__,__LINE__,__func__,A,B,C)
void printf_and_exit(int exitstatus,const char* format,...){
	va_list va;
	va_start(va,format);
	vprintf(format,va);
	va_end(va);
	exit(exitstatus);
}
void print_error_with_pos(const char* file,int line,const char* func,int exitstatus,const char* who,const char* why){
	printf_and_exit(exitstatus,"%s:%d::%s\t%s failed for %s\n",file,line,func,who,why);
}
void socket_connect(int& svrfd,const int port){
	struct sockaddr_in svraddr;
	int re;
	if((svrfd=socket(AF_INET,SOCK_STREAM,0))<0){
		print_error(1,"socket",strerror(errno));
	}
	bzero(&svraddr,sizeof(svraddr));
	svraddr.sin_family=AF_INET;
	svraddr.sin_port=htons(port);
	if((re=connect(svrfd,(struct sockaddr*)(&svraddr),sizeof(svraddr)))<0){
		print_error(1,"connect",strerror(errno));
	}
}
void read_write(const int& fromfd,const int& tofd,int& eof){
	int re;
	char buff[MAXLINE];
	if((re=read(fromfd,buff,sizeof(buff)))<0){
		print_error(1,"select",strerror(errno));
	}else if(re==0){//some data may not be received
		//printf_and_exit(0,"%s %s\n",who,exitstr);
		printf("showdown\n");
		shutdown(tofd,SHUT_WR);
		eof=1;
	}
	int len=re;
	if(!eof && (re=write(tofd,buff,len))!=len){
		print_error(1,"select",strerror(errno));
	}
}
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
	socket_connect(svrfd,port);
	
	int stdineof=0;
	int svrfdeof=0;
	struct pollfd fds[2];
	fds[0].fd=stdinfd;
	fds[0].events=POLLRDNORM;
	fds[1].fd=svrfd;
	fds[1].events=POLLRDNORM;
	while(1){
		if((re=poll(fds,2,INT_MAX))<0){
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
