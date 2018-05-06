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
	int stdinfd=fileno(stdin);
	int stdoutfd=fileno(stdout);
	fd_set rset,allset; 
	socket_connect(svrfd,25000);
	
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
			read_write(stdinfd,svrfd,stdineof);
		}
		if(!svrfdeof && FD_ISSET(svrfd,&rset)){
			//printf("svrfd ready\n");
			read_write(svrfd,stdoutfd,svrfdeof);
			if(svrfdeof){
				exit(0);
			}
		}
	}
}
