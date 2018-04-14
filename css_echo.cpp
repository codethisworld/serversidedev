#include<stdio.h>
#include<netinet/ip.h>//sockaddr_in
#include<sys/socket.h>//socket
#include<string.h>//bzero strerror
#include<stdarg.h>//va_list va_start
#include<stdlib.h>//exit
#include<errno.h>
#include<unistd.h>//fork
#include<sys/wait.h>//waitpid
#include<pthread.h>
////////////////////////////////////variables///////////////////////////
pthread_mutex_t mutex;
////////////////////////////////////function///////////////////////////////
void printf_and_exit(int exitstatus,const char* format,...){
	va_list va;
	pthread_mutex_lock(&mutex);
	printf("in %25d:",getpid());
	va_start(va,format);
	vprintf(format,va);
	pthread_mutex_unlock(&mutex);
	va_end(va);
	exit(exitstatus);
}
void concurrent_printf(pthread_mutex_t* pmutex,const char* format,...){
	va_list va;
	va_start(va,format);
	pthread_mutex_lock(pmutex);
	vprintf(format,va);
	pthread_mutex_unlock(pmutex);
	va_end(va);
}
////////////////////////////////////main///////////////////////////////////
int main(){
	struct sockaddr_in svraddr;
	int svrfd;
	int re;
	int working=0;
	pthread_mutex_init(&mutex,NULL);
	concurrent_printf(&mutex,"in %25d:main begin\n",getpid());
	svrfd=socket(AF_INET,SOCK_STREAM,0);
	bzero(&svraddr,sizeof(svraddr));
	svraddr.sin_port=htons(25000);
	svraddr.sin_family=AF_INET;
	if((re=bind(svrfd,(struct sockaddr*)&svraddr,sizeof(svraddr)))<0){
		printf_and_exit(1,"%s failed for:%s\n","bind",strerror(errno));
	}
	listen(svrfd,1024);
	working=1;
	while(working){
		struct sockaddr_in cliaddr;
		int addrlen;
		int clifd;
		int pid;
		if((clifd=accept(svrfd,(struct sockaddr*)&cliaddr,(socklen_t*)&addrlen))<0){
			printf_and_exit(1,"%s failed for:%s\n","accept",strerror(errno));
		}
		if((pid=fork())==0){
			concurrent_printf(&mutex,"in %25d:son begin\n",getpid());
			int childpid;
			if((childpid=fork())>0){
				concurrent_printf(&mutex,"in %25d:son end\n",getpid());
				exit(0);
			}
			concurrent_printf(&mutex,"in %25d:grandson begin\n",getpid());
			int len;
			char buff[1024];
			buff[sizeof(buff)-1]=0;
			while((len=read(clifd,buff,sizeof(buff)-1))>0){
				printf("hisin_log,len:%d server read and write:%s\n",len,buff);
				if(write(clifd,buff,sizeof(buff))<0){
					printf_and_exit(1,"%s failed for:%s\n","write",strerror(errno));
				}
				buff[sizeof(buff)-1]=0;
			}
			if(len<0){
				printf_and_exit(1,"%s failed for:%s\n","read",strerror(errno));
			}
			close(clifd);
			concurrent_printf(&mutex,"in %25d:grandson end\n",getpid());
			exit(0);
		}else{
			int status;
			if(waitpid(pid,&status,0)<0){
				printf_and_exit(1,"%s failed for:%s\n","waitpid",strerror(errno));
			}
		}
	}
	close(svrfd);
	concurrent_printf(&mutex,"in %25d:end\n",getpid());
	pthread_mutex_destroy(&mutex);
	return 0;
}
