#include <cstring>
#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "lhf.h"
#include "socketio.h"
using namespace std;
int gport=25000;
////////////////////functions///////////////
void parse_options(int argc,char *argv[]){
	char ch;
	while((ch=getopt(argc,argv,""))!=-1){
		switch(ch){
			case 'P':
				gport=atoi(optarg);
				break;
			case '?':
				print_error(1,__func__,"unknown option:%c\n",optopt);
				break;
			case ':':
				print_error(1,__func__,"option %c need value\n",optopt);
				break;
			default:
				printf_log_c99("info","unsolved option %c",ch);
				break;
		}
	}
}
////////////////////main////////////////////
int main(int argc,char* argv[]){
	int srvfd,epollfd;
	int nfds;
	int re;
	struct epoll_event ev,events[MAX_EVENTS];
	parse_options(argc,argv);
	if((re=socket_bind_listen_epoll_create_epoll_ctl(srvfd,gport,epollfd,ev))<0){
		print_error(1,"socket_bind_listen_epoll_create_epoll_ctl","");
	}
	while(1){
		nfds=epoll_wait(epollfd,events,MAX_EVENTS,-1);	
		if(nfds<0){
				print_error(2,"epoll_wait",strerror(errno));
		}
		for(int i=0;i<nfds;i++){
			if(events[i].data.fd==srvfd){
				if((re=accept_epoll_ctl(epollfd,srvfd))){
					print_error(3,"accept_epoll_ctl","");
				}
			}else{
				if((re=deal_epoll_event(epollfd,events+i,date_echo,NULL))){
					print_error(4,"deal_epoll_event","");
				}
			}
		}
	}
	return 0;
}
