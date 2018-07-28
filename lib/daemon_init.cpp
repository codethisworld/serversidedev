#include <stdio.h>
#include "lhf.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>//close getcwd chdir
#include <syslog.h>//openlog
#include <signal.h>//SIG_IGN
#include <fcntl.h>//O_RDONLY
////////////////////variables////////////////////
enum {MAXDESCRIPTOR=64};
////////////////////functions////////////////////
void fork_and_reserve_child_process(){
    int re;
	printf_log_c99("debug","in %ld, begin to fork\n",getpid());
    if((re=fork())<0){
        print_error(1,"fork",strerror(errno));
    }
    if(re>0){
		printf_log_c99("debug","in %ld, begin to exit\n",getpid());
        _exit(0);
    }
	printf_log_c99("debug","in %ld, fork ok\n",getpid());
}
int daemon_init(const char *pname,int facility){
    int re;
    int i;
    char buff[MAXLINE];
    //1. remove control terminal
    fork_and_reserve_child_process();
    if((re=setsid())<0){
        print_error(1,"setsid",strerror(errno));
    }
    signal(SIGHUP,SIG_IGN);
    fork_and_reserve_child_process();
    //2. set message to syslog instead of stdin stdout stderr
    //daemon_proc=1;//for err_xxx() functions
    //3. chdir
    if(getcwd(buff,sizeof(buff))==NULL){
        print_error(1,"setsid",strerror(errno));
    }
    //chdir(buff);
    if(chdir("/")<0){
        print_error(1,"setsid",strerror(errno));
    }
    //4. close all opened descriptor
	printf("%10s %d %ld\n","stdin",STDIN_FILENO,stdin);
	printf("%10s %d %ld\n","stdout",STDOUT_FILENO,stdout);
	printf("%10s %d %ld\n","stderr",STDERR_FILENO,stderr);
    for(i=3;i<MAXDESCRIPTOR;i++){
		close(i);
    }
    //5. set stdin stdout stderr to /dev/null
   // if(open("/dev/null",O_RDONLY)<0){
   //     print_error(1,"setsid",strerror(errno));
   // }
   // if(open("/dev/null",O_RDWR)<0){
   //     print_error(1,"setsid",strerror(errno));
   // }
   // if(open("/dev/null",O_RDWR)<0){
   //     print_error(1,"setsid",strerror(errno));
   // }
    //6. openlog
    openlog(pname,LOG_PID,facility);
    return 0; 
}
////////////////////main////////////////////
