#include <unistd.h>
#include <cstring>
#include <errno.h>

#include "lhf.h"
using namespace std;
////////////////////variables///////////////
enum {MAX_EVENTS=20};
////////////////////declerations////////////
////////////////////functions///////////////
void parse_options(int argc,char *argv[]){
	char ch;
	while((ch=getopt(argc,argv,""))!=-1){
		switch(ch){
			case '':
				break;
			case '':
				break;
			case '':
				break;
			case '':
				break;
			case '':
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
int socket_bind_listen_epoll_create_epoll_ctl(int &srvfd,int &epollfd,struct epoll_event &ev){
	int re;
	if(socket_bind_listen(srvfd)){
		printf_log_c99("debug","%s failed\n","socket_bind_listen")
		return 1;
	}
	if((epollfd=epoll_create(0))<0){
		printf_log_c99("debug","%s failed for:%s\n","epoll_create",strerror(errno));
		return 2;
	}
	if(epoll_add(epollfd,srvfd,EPOLLIN)){
		printf_log_c99("debug","%s failed\n","epoll_add");
		return 3;
	}
	return 0;
}
int epoll_add(int epollfd,int addfd,uint32_t events){
	struct epoll_event ev;
	int re;
	ev.data.fd=addfd;
	ev.events=events;
	if((re=epoll_ctl(epollfd,EPOLL_CTL_ADD,addfd,&ev))<0){
		printf_log_c99("debug","%s failed for:%s\n","epoll_ctl",strerror(errno))
		return 1;
	}
	return 0;
}
int accept_epoll_ctl(int epollfd,int srvfd){
	int clifd;
	if((clifd=accept(srvfd,NULL,NULL))<0){
		printf_log_c99("fail","%s failed for:%s\n","accept",strerror(errno));
		return 1;
	}
	if(epoll_add(epollfd,clifd,EPOLLIN)){
		printf_log_c99("debug","%s failed\n","epoll_add");
		return 2;
	}
	return 0;
}
int deal_epoll_event(struct epoll_event *resultev){
	if((resultev->events)&EPOLLIN){
		if(date_echo(resultev->data.fd)){
			printf_log_c99("debug","%s failed\n","date_echo");
			return 1;
		}
	}else{
		printf_log_c99("debug","%s failed for: unknown event %d\n","deal_epoll_event",resultev->events);
		return 2;
	}
	return 0;
}
int date_echo(int fd){
	time_t t=time(NULL);
	char buff[MAXLINE];
	int len=snpfrintf(buff,sizeof(buff),ctime(&t));//size include '\0',len not include '\0'
	if(full_write(fd,buff,len)){
		printf_log_c99("debug","%s failed\n","full_write");
		return 1;
	}
	return 0;
}
int full_write(int fd,char* buff,len){
	char* pbuff=buff;
	int re;
	while((re=write(fd,pbuff,len))>0){
		if(re<len){
			printf_log_c99("debug","len:%d write:%d left:%d\n",len,re,len-re);
			pbuff+=re;
			len-=re;
		}else{
			break;
		}
	}
	if(re<0){
		printf_log_c99("debug","%s failed for:%s\n","write",strerror(errno));
		return 1;
	}
	assert(re==0);
	return 0;	
}
////////////////////main////////////////////
int main(int argc,char* argv[]){
	int srvfd,epollfd;
	int nfds;
	int re;
	struct epoll_event ev,events[MAX_EVENTS];
	parse_options(argc,argv);
	if((re=socket_bind_listen_epoll_create_epoll_ctl(srvfd,epollfd,ev))<0){
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
				if((re=deal_epoll_event())){
					print_error(4,"deal_epoll_event","");
				}
			}
		}
	}
	return 0;
}
