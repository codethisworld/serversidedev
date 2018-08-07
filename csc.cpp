#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h>//sockaddr_in
#include<arpa/inet.h>
#include<string.h>
#include<stdarg.h>//va_start va_end
#include<unistd.h>//read
#include<errno.h>//errno
#include<sys/types.h>
#include<sys/socket.h>
//////////////////////////////////////function//////////////////////////////
void printf_and_exit(int exit_status,const char* format,...){
	va_list va;
	va_start(va,format);
	vprintf(format,va);//cstdio
	va_end(va);
	exit(exit_status);
}
//////////////////////////////////////main//////////////////////////////////
int main(){
	int fd;
	int re;
	struct sockaddr_in svraddr;
	char line[1024];
	int n;
	fd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&svraddr,sizeof(svraddr));
	svraddr.sin_family=AF_INET;
	svraddr.sin_port=htons(25000);
	//inet_pton(AF_INET,"127.0.0.1",&(svraddr.sin_addr));
	if( (n=connect(fd,(struct sockaddr*)&svraddr,sizeof(svraddr))) <0){
		printf_and_exit(1,"%s failed for:%s\n","connect",strerror(errno));
	}
	while(1){
		if((re=read(fd,line,sizeof(line)-1))>0){
			line[1023]=0;
			if(fputs(line,stdout)==EOF){
				printf_and_exit(1,"%s failed for:%s\n","fputs","return EOF");
			}
		}
		if(re<0){
			printf_and_exit(1,"%s failed for:%s\n","read",strerror(errno));
		}
	}
	printf_and_exit(0,"success\n");
}
