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
void sigchld_func(int signo){
	if(signo==SIGCHLD){
		int status;
		int pid;
		struct sigaction act;
		sigaction(SIGCHLD,NULL,&act);
		if( (act.sa_flags)&SA_NOCLDWAIT ){
			printf("the sa_flags of SIGCHLD have SA_NOCLDWAIT option,it will not be zombie porcess,skip to use waitpid\n");
		}
		while((pid=waitpid(-1,&status,WNOHANG))>0){
			printf("%d:%d exit\n",getpid(),pid);
		}
		//if(pid<0 && errno!= ECHILD){
		//	printf_and_exit(1,"%s failed for:%s\n","waitpid",strerror(errno));
		//}
	}
}
struct sigaction mysignal(int signo,void (*func)(int)){
	struct sigaction act,oact;
	act.sa_handler=func;
	sigemptyset(&act.sa_mask);//don't block other signal when this function is running.but signal with the same signo of this fuction is block,so other xxtra signal with the same signo is lost becasuse of no signal queue in unix
	act.sa_flags=0;
	sigaction(signo,NULL,&oact);
	if(oact.sa_handler!=SIG_IGN){
		sigaction(signo,&act,&oact);
	}else{
		printf("[%s] handler is SIG_IGN",strsignal(signo));
	}
	return oact;
}
////////////////////////////////////main///////////////////////////////////
int main(){
	struct sockaddr_in svraddr;
	int svrfd;
	int re;
	int working=0;
	mysignal(SIGCHLD,sigchld_func);
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
			if(errno==EINTR){//when prcess func of SIGCHLD return,EINTR may be generated because system is blocked by accept before SIGCHLD
				continue;
			}else{
				printf_and_exit(1,"%s failed for:%s\n","accept",strerror(errno));
			}
		}
		if((pid=fork())==0){
			concurrent_printf(&mutex,"in %25d:son begin\n",getpid());
			int childpid;
			if((childpid=fork())>0){
				concurrent_printf(&mutex,"in %25d:son end\n",getpid());
				_exit(0);
			}
			concurrent_printf(&mutex,"in %25d:grandson begin\n",getpid());
			int len;
			char buff[1024];
			buff[sizeof(buff)-1]=0;
			while((len=read(clifd,buff,sizeof(buff)-1))>0){
				if(write(clifd,buff,len)<0){
					printf_and_exit(1,"%s failed for:%s\n","write",strerror(errno));
				}
				printf("hisin_log,len:%d server read and write:%s\n",len,buff);
				buff[sizeof(buff)-1]=0;
			}
			if(len<0){
				printf_and_exit(1,"%s failed for:%s\n","read",strerror(errno));
			}
			close(clifd);
			concurrent_printf(&mutex,"in %25d:grandson end\n",getpid());
			_exit(0);
		}
	}
	close(svrfd);
	concurrent_printf(&mutex,"in %25d:end\n",getpid());
	pthread_mutex_destroy(&mutex);
	return 0;
}
