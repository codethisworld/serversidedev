#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include "lib/lhf.h"
/////////////////////////declerations/////////////////////////////
void sendto_recvfrom_fputs_after_connect(int srvfd,char buff[],int len,FILE* out);

/////////////////////////functions/////////////////////////////////
void sendto_recvfrom_fputs_after_connect(int srvfd,char buff[],int len,FILE* out){
	int buffsize=MAXLINE;
	int re;
	if((re=sendto(srvfd,buff,len,0,NULL,0))<0){
		print_error(1,"sendto",strerror(errno));
	}
	printf_log_c99("debug","sendto:%d",re);
	printf_log_c99("debug","send:%s",buff);
	bzero(buff,sizeof(buff));
	if((re=recvfrom(srvfd,buff,buffsize-1,0,NULL,0))<0){
		print_error(1,"recvfrom",strerror(errno));
	}
	printf_log_c99("debug","recvfrom:%d",re);
	printf_log_c99("debug","receive[buffsize-1:%d]:%s",buffsize-1,buff);
	fputs(buff,out);
}
void udp_close_connection(int sockfd){
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_UNSPEC;
	if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr))<0){
		print_error(1,"connect",strerror(errno));
	}
}
//////////////////////////main//////////////////////////////////////
int main(){
	int  srvfd;
	int len;
	struct sockaddr_in srvaddr;
	char buff[MAXLINE];
	if((srvfd=socket(AF_INET,SOCK_DGRAM,0))<0){
		print_error(1,"socket",strerror(errno));
	}
	bzero(&srvaddr,sizeof(srvaddr));
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=htons(25000);
	if(inet_aton("127.0.0.1",&(srvaddr.sin_addr))==0){
		print_error(1,"inet_aton","invalid address:%s","127.0.0.1");
	}
	if(connect(srvfd,(struct sockaddr*)(&srvaddr),sizeof(srvaddr))<0){
		print_error(1,"connect",strerror(errno));
	}
	while((fgets(buff,sizeof(buff),stdin))!=NULL){
		len=strlen(buff)+1;
		sendto_recvfrom_fputs_after_connect(srvfd,buff,len,stdout);
	}
	udp_close_connection(srvfd);
	return 0;
}
