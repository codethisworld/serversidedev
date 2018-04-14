#include<stdio.h>
#include<stdlib.h>//exit
#include<netinet/ip.h>//sockaddr_in
#include<sys/socket.h>//socket
#include<string.h>//bzero memset strerror
#include<stdarg.h>//va_list va_start va_end
#include<errno.h>//errno
#include<unistd.h>//write read
////////////////////////////////////////////function///////////////////////////////////
void printf_and_exit(int exitstatus,const char* format,...){
	va_list va;
	va_start(va,format);
	vprintf(format,va);
	va_end(va);
	exit(exitstatus);
}
////////////////////////////////////////////main//////////////////////////////////////
int main(){
	struct sockaddr_in svraddr;
	int fd;
	int re;
	char buff[1024];
	int working=0;
	fd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&svraddr,sizeof(svraddr));
	svraddr.sin_port=htons(25000);
	svraddr.sin_family=AF_INET;
	if((re=connect(fd,(struct sockaddr*)&svraddr,sizeof(svraddr)))<0){
		printf_and_exit(1,"%s failed for:%s\n","connect",strerror(errno));
	}
	working=1;
	while(working){
		if(fgets(buff,sizeof(buff),stdin)==NULL){
			printf_and_exit(1,"%s failed for:%s\n","fgets",ferror(stdin));
		}
		int len=strlen(buff);
		if((re=write(fd,buff,len))<0){
			printf_and_exit(1,"%s failed for:%s\n","write",strerror(errno));
		}
		printf("hisin_log,re:%d client fgets and write:%s\n",re,buff);
		buff[sizeof(buff)-1]=0;
		int cnt=0;
		while((re=read(fd,buff,sizeof(buff)-1))>=0){
			buff[re]=0;
			printf("hisin_log,re:%d client read and fputs:%s\n",re,buff);
			fputs(buff,stdout);
			cnt+=re;
			if(cnt==len){
				break;
			}else if(cnt>len){
				printf_and_exit(1,"%s failed for:%s\n","read","cnt[%d]>len[%d]",cnt,len);
			}
		}
		if(re<0){
			printf_and_exit(1,"%s failed for:%s\n","read",strerror(errno));
		}

	}
	return 0;
}
