#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <assert.h>
#include "lib/lhf.h"
/////////////////////////declerations/////////////////////////////

/////////////////////////functions/////////////////////////////////
void udp_socket_bind(int &srvfd,int port){
	struct sockaddr_in srvaddr;
	if((srvfd=socket(AF_INET,SOCK_DGRAM,0))<0){
		print_error(1,"socket",strerror(errno));
	}
	bzero(&srvaddr,sizeof(srvaddr));
	srvaddr.sin_family=AF_INET;	
	srvaddr.sin_port=htons(port);
	if((bind(srvfd,(struct sockaddr*)&srvaddr,(socklen_t)sizeof(srvaddr)))<0){
		print_error(1,"bind",strerror(errno));
	}
}
void udp_echo_connect(const int srvfd){
	int len;
	char buff[MAXLINE];
	struct sockaddr_in cliaddr;
	int cliaddrlen=sizeof(cliaddr);
	int re;
	if((len=recvfrom(srvfd,buff,sizeof(buff),0,(struct sockaddr*)&cliaddr,(socklen_t*)&cliaddrlen))<0){
		print_error(1,"recvfrom",strerror(errno));
	}
	printf_log_c99("debug","recvfrom:%d",len);
	printf_log_c99("debug","server recvfrom:[%d]%s",len,buff);
	if((re=sendto(srvfd,buff,len,0,(struct sockaddr*)&cliaddr,cliaddrlen))<0){
		print_error(1,"sendto",strerror(errno));
	}
	printf_log_c99("debug","sendto:%d",re);
}
/////////////////////////main//////////////////////////////////////
int main(){
	int  srvfd;
	int re;
	udp_socket_bind(srvfd,25000);
	////this is ok
	//while(1){
	//	udp_echo_connect(srvfd);
	//}
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(srvfd,&fds);
	fd_set fds1=fds;
	while((re=select(srvfd+1,&fds1,NULL,NULL,NULL))>0){
		assert(FD_ISSET(srvfd,&fds1));
		udp_echo_connect(srvfd);
		fds1=fds;
	}
	if(re<0){
		print_error(1,"select",strerror(errno));
	}
	return 0;
}
