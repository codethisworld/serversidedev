#include "lhf.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
////////////////////variables////////////////////
////////////////////functions////////////////////
////////////////////main////////////////////
int main(int argc,char* argv[]){
	int re,len;
	int srvfd,clifd;
	time_t t;
	char buff[MAXLINE];
	socklen_t addrlen;
	if(daemon_init("daytimetcpsrv2",0)){
		print_error(1,"daemon_init","");
	}
	printf_log_c99("debug","daemon_init ok\n");
	if((re=tcp_listen("localhost","25000",&addrlen))<0){
		print_error(1,"tcp_listen","");
	}
	printf_log_c99("debug","tcp_listen ok\n");
	srvfd=re;
	while(1){
		if((clifd=accept(srvfd,NULL,NULL))<0){
			print_error(1,"accept",strerror(errno));
		}
		printf_log_c99("debug","accept ok\n");
		t=time(NULL);
		if((len=snprintf(buff,sizeof(buff),"%s\n",ctime(&t)))<0){
			print_error(1,"snprintf","");
		}
		if((re=write(clifd,buff,len+1))<0){
			print_error(1,"snprintf","");
		}
		printf_log_c99("debug","write ok\n");
		close(clifd);
	}
}
