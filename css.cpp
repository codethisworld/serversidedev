#include<stdio.h>
#include<netinet/ip.h>//sturct sockaddr_in
#include<sys/socket.h>
#include<stdarg.h>//va_start va_end
#include<stdlib.h>//exit
#include<string.h>//strerror
#include<errno.h>//errno
#include<unistd.h>//fork
#include<time.h>//time localtime asctime
////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////function/////////////////////////////////////////
void printf_and_exit(int exitstatus,const char* format,...){
	va_list va;
	va_start(va,format);
	vprintf(format,va);
	va_end(va);
	exit(exitstatus);
}
void write_time(int fd){
	char buff[1024];
	int re;
	time_t rawtime=time(NULL);
	sprintf(buff,"%s\n",asctime(localtime(&rawtime)));
	if((re=write(fd,buff,sizeof(buff)))<0){
		printf_and_exit(1,"%s failed for: %s\n","write",strerror(errno));
	}
}
void write_to_fd_and_close_it(int fd,int time){
	char buff[1024];
	int re;
	for(int i=0;i<time;i++){
		sprintf(buff,"%d : %d\n",getpid(),i);
		if((re=write(fd,buff,sizeof(buff)))<0){
			printf_and_exit(1,"%s failed for: %s\n","write",strerror(errno));
		}
		sleep(1);
	}
	close(fd);
	printf("%d,close fd\n",getpid());
}
//////////////////////////////////////main///////////////////////////////////////////
int main(){
	int svrfd;
	int clifd;
	struct sockaddr_in svraddr;
	int re;
	int working=0;
	svrfd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&svraddr,sizeof(svraddr));	
	svraddr.sin_family=AF_INET;
	svraddr.sin_port=htons(25000);
	if((re=bind(svrfd,(struct sockaddr *)&svraddr,sizeof(svraddr))) <0){
		printf_and_exit(1,"%s failed for: %s\n","bind",strerror(errno));
	}
	if((re=listen(svrfd,1024))<0){
		printf_and_exit(1,"%s failed for: %s\n","listen",strerror(errno));
	}
	working=1;
	while(working){
		struct sockaddr_in cliaddr;
		int pid;
		int len=0;
		if((clifd=accept(svrfd,(struct sockaddr*)&cliaddr,(socklen_t *)&len)) < 0){
			printf_and_exit(1,"%s failed for: %s\n","accept",strerror(errno));
		};
		if((pid=fork())==0){
			write_to_fd_and_close_it(clifd,20);
			//write_time(clifd);
			//close(clifd);
		}
		write_to_fd_and_close_it(clifd,10);
	}
	close(svrfd);
	return 0;
}
