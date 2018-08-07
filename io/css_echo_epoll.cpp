#include <cstring>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "lhf.h"
#include "socketio.h"
using namespace std;
int opport=25000;
////////////////////functions///////////////
void parse_options(int argc,char *argv[]){
	char ch;
	printf("argc:%d\n",argc);
	while((ch=getopt(argc,argv,"P:"))!=-1){
		switch(ch){
			case 'P':
				opport=atoi(optarg);
				break;
			default:
				print_fail_reason("debug",__func__,"return %c",optopt);
				if(parse_options_common(ch,optopt,optarg,optind,opterr)){
					print_error(1,__func__,"");
				}
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
	if((re=socket_bind_listen_epoll_create_epoll_ctl(srvfd,opport,epollfd,ev))){
		print_error(1,"socket_bind_listen_epoll_create_epoll_ctl","return %d",re);
	}
	while(1){
		nfds=epoll_wait(epollfd,events,MAX_EVENTS,-1);	
		if(nfds<0){
				print_error(2,"epoll_wait",strerror(errno));
		}
		for(int i=0;i<nfds;i++){
			if(events[i].data.fd==srvfd){
				if((re=accept_epoll_ctl(epollfd,srvfd))){
					print_error(3,"accept_epoll_ctl","return %d",re);
				}
			}else{
				if((re=deal_epoll_event(epollfd,events+i,data_echo,NULL))){
					print_error(4,"deal_epoll_event","return %d",re);
				}
			}
		}
	}
	close(srvfd);
	close(epollfd);
	return 0;
}
