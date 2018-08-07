#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include "lib/lhf.h"
///////////////////////////////////////////declerations/////////////////////////////
void sendto_recvfrom_fputs(int srvfd,char buff[],int len,struct sockaddr* srvaddr,int& addrlen,FILE* out);

///////////////////////////////////////////functions/////////////////////////////////
void sendto_recvfrom_fputs(int srvfd,char buff[],int len,struct sockaddr* srvaddr,int& addrlen,FILE* out){
	printf_log_c99("debug","sizeof(buff) after enter sendto_recvfrom_fputs:%d\n",sizeof(buff));
	printf_log_c99("info","attention:sizeof(char buff[]):%d is just sizeof(char*):%d\n",sizeof(buff),sizeof(char*));
	int buffsize=MAXLINE;
	if((sendto(srvfd,buff,len,0,srvaddr,addrlen))<0){
		print_error(1,"sendto",strerror(errno));
	}
	printf_log_c99("debug","send:%s",buff);
	printf_log_c99("debug","sizeof(buff) after endto and before recvfrom:%d\n",sizeof(buff));
	bzero(buff,len);
	struct sockaddr_in replyaddr;
	int replyaddrlen=addrlen;
	bzero(&replyaddr,sizeof(replyaddr));
	if((recvfrom(srvfd,buff,buffsize-1,0,(struct sockaddr*)&replyaddr,(socklen_t*)&replyaddrlen))<0){
		print_error(1,"recvfrom",strerror(errno));
	}
	printf_log_c99("debug","receive[,buffsize-1:%d]:%s",buffsize-1,buff);
	if(replyaddrlen !=addrlen || (memcmp(srvaddr,&replyaddr,addrlen)!=0)){
		printf("ignore package from unknown host:%d<=>%d %s<=>%s\n",addrlen,replyaddrlen,inet_ntoa(((struct sockaddr_in *)srvaddr)->sin_addr),inet_ntoa(replyaddr.sin_addr));
		hex_dump(srvaddr,addrlen);
		hex_dump(&replyaddr,replyaddrlen);
	}else{
		fputs(buff,out);
	}
}
///////////////////////////////////////////main//////////////////////////////////////
int main(){
	int  srvfd;
	int len;
	char buff[MAXLINE];
	printf_log_c99("debug","sizeof(buff) after decleration:%d\n",sizeof(buff));
	if((srvfd=socket(AF_INET,SOCK_DGRAM,0))<0){
		print_error(1,"socket",strerror(errno));
	}
	while((fgets(buff,sizeof(buff),stdin))!=NULL){
		struct sockaddr_in src_addr;
		bzero(&src_addr,sizeof(src_addr));
		src_addr.sin_family=AF_INET;
		src_addr.sin_port=htons(25000);
		if(inet_pton(AF_INET,"127.0.0.2",&(src_addr.sin_addr))<0){
			print_error(1,"inet_pton",strerror(errno));
		}
		int addrlen=sizeof(src_addr);
		len=strlen(buff);
		printf_log_c99("debug","input[%d]:%s",len,buff);
		printf_log_c99("debug","sizeof(buff) after fgets and before sendto_recvfrom_fputs:%d\n",sizeof(buff));
		sendto_recvfrom_fputs(srvfd,buff,len,(struct sockaddr*)&src_addr,addrlen,stdout);
	}
	return 0;
}
