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
enum {MAXFDSIZE=500,MAXLINE=1024};
//////////////////////////functions///////////////////////////////////
void printf_and_exit(int exitstatus,const char* format,...){
	va_list va;
	va_start(va,format);
	vprintf(format,va);
	va_end(va);
	exit(exitstatus);
}
void print_error(int exitstatus,const char* who,const char* why){
	printf_and_exit(exitstatus,"%s failed for %s\n",who,why);
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
void read_write(const int& fromfd,const int& tofd,const char* who,const char* exitstr){
	int re;
	char buff[MAXLINE];
	if((re=read(fromfd,buff,sizeof(buff)))<0){
		print_error(1,"select",strerror(errno));
	}else if(re==0){//some data may not be received
		printf_and_exit(0,"%s %s\n",who,exitstr);
	}
	int len=re;
	if((re=write(tofd,buff,len))!=len){
		print_error(1,"select",strerror(errno));
	}
}
//////////////////////////main////////////////////////////////////////
int main(int argc,char* argv[]){
	int svrfd;
	int re;
	int stdinfd=fileno(stdin);
	fd_set rset,allset; 
	socket_connect(svrfd,25000);
	
	FD_ZERO(&allset);
	FD_SET(stdinfd,&allset);
	FD_SET(svrfd,&allset);
	int maxfd=svrfd;
	if(stdinfd>maxfd){
		maxfd=stdinfd;
	}
	while(1){
		rset=allset;
		if((re=select(maxfd+1,&rset,NULL,NULL,NULL))<0){
			print_error(1,"select",strerror(errno));
		}
		if(re==0){
			continue;	
		}
		if(FD_ISSET(stdinfd,&rset)){
			read_write(stdinfd,svrfd,argv[0],"exit normally");
		}
		if(FD_ISSET(svrfd,&rset)){
			read_write(svrfd,stdinfd,argv[0],"close by the server");
		}
	}
}
