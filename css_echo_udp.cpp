#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include "lib/lhf.h"
///////////////////////////////////////////declerations/////////////////////////////

///////////////////////////////////////////functions/////////////////////////////////
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
void udp_echo(const int srvfd,struct sockaddr* pcliaddr,int& addrlen){
	int len;
	char buff[MAXLINE];
	if((len=recvfrom(srvfd,buff,sizeof(buff),0,pcliaddr,(socklen_t*)&addrlen))<0){
		print_error(1,"recvfrom",strerror(errno));
	}
	printf_log_c99("debug","server recvfrom:[%d]%s",len,buff);
	if((sendto(srvfd,buff,len,0,pcliaddr,addrlen))<0){
		print_error(1,"sendto",strerror(errno));
	}
}
///////////////////////////////////////////main//////////////////////////////////////
int main(){
	int  srvfd;
	udp_socket_bind(srvfd,25000);
	while(1){
		struct sockaddr_in src_addr;
		int addrlen=sizeof(src_addr);
		udp_echo(srvfd,(struct sockaddr*)&src_addr,addrlen);
	}
	return 0;
}
