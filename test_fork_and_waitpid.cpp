#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>//_exit
#include <signal.h>
#include <sys/wait.h>//waitpid
#include <pthread.h>
#include <stdarg.h>
#include <time.h>
///////////////////////////////global variables///////////////////////////
//glibc里面的printf是线程安全的，单次printf不会被截断。要想保证某个线程的多个printf连续，需要加flockfile/funlockfile
//[stdout thread-safe in C on Linux?](https://stackoverflow.com/questions/467938/stdout-thread-safe-in-c-on-linux)
pthread_mutex_t gmutex;
int step_num=0;// 各个进程中第一条输出都标号为1，所以全局变量也会拷贝,也是进程间独立的，所以上面那个pthread_mutex_t是无效的

///////////////////////////////function///////////////////////////////////
void concurrent_printf(pthread_mutex_t *mutex,const char* format,...){
	va_list va;
	va_start(va,format);
	//pthread_mutex_lock(mutex);
	printf("%d ",++step_num);
	vprintf(format,va);
	fflush(stdout);
	//pthread_mutex_unlock(mutex);
	va_end(va);
}
void waitchild(){
	int pid;
	int status;
	// 不能用wait是因为wait没有WNOHANG这个选项，在while中会阻塞
	while((pid=waitpid(-1,&status,WNOHANG))>0){// 一次信号，可能处理当前所有结束的子进程；当前未结束的，由后面的信号处理；
		concurrent_printf(&gmutex,"in %d,child process %d exit with status %d\n",getpid(),pid,status);
	}
	if(pid<0 && (errno!=ECHILD)){// 处理ECHILD这个异常情况
		concurrent_printf(&gmutex,"in %d, waitpid failed for:%s \n",getpid(),strerror(errno));
		exit(pid);
	}
	return ;
}
void sigfunc(int signo){
	concurrent_printf(&gmutex,"in sigfunc\n");
    if(signo==SIGCHLD){
		waitchild();
    }
}
void wait_all_children(){
	int pid;
	int status;	
	while((pid=waitpid(-1,&status,WNOHANG))>=0){
		if(pid>0){
			concurrent_printf(&gmutex,"in %d,child process %d exit with status %d\n",getpid(),pid,status);
		}else if(pid==0){
			sleep(1);
		}
	}
	if(pid<0 && (errno!=ECHILD)){
		concurrent_printf(&gmutex,"in %d, waitpid failed for:%s \n",getpid(),strerror(errno));
		exit(pid);
	}
}
struct sigaction mysignal(int signo,void (*func)(int)){
    struct sigaction act,oact;
    act.sa_handler=func;
    sigemptyset(&(act.sa_mask));// func运行期间，不阻塞其他的信号（只阻塞与signo相同的信号）
    act.sa_flags=0;
    if(sigaction(signo,&act,&oact)<0){
        concurrent_printf(&gmutex,"in %d, %s failed for %s\n",getpid(),"waitpid",strerror(errno));
		//act.sa_handler=SIG_DFL;
        //sigaction(signo,&act,NULL);
        exit(errno);
    }
    return oact;
}
///////////////////////////////main///////////////////////////////////////
int main(){
	pthread_mutex_init(&gmutex,NULL);
	mysignal(SIGCHLD,sigfunc);
	for(int i=0;i<4;i++){
		int pid=1;
		if((pid=fork())<0){
			concurrent_printf(&gmutex,"in %d, %s failed for %s\n",getpid(),"fork",strerror(errno));
			exit(errno);
		}else if(pid==0){
			concurrent_printf(&gmutex,"in child %d\n",getpid());
			exit(0);
		}
	}
	concurrent_printf(&gmutex,"in parent %d,before sleep(20),time:%d\n",getpid(),time(NULL));
	int lefttime=0;
	if((lefttime=sleep(20))==0){
		concurrent_printf(&gmutex,"in parent %d,sleep success\n");
	}else{
		concurrent_printf(&gmutex,"in parent %d,sleep returned [for %s],remain %d second not sleep\n",getpid(),"a signal arrives which is not ignored",lefttime);
	}
	// sleep until seconds seconds have elapsed or a signal arrives which is not ignored
	concurrent_printf(&gmutex,"in parent %d,after  sleep(20),time:%d\n",getpid(),time(NULL));
	wait_all_children();
	pthread_mutex_destroy(&gmutex);
	return 0;
}
