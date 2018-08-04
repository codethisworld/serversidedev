#include <sys/epoll.h>
#include "lhf.h"
#include "socketio.h"
/////////////////////////////////////////func//////////////////////////////////////////
int socket_bind_listen(int& svrfd,const int port){
	int re;
	struct sockaddr_in svraddr;
	bzero(&svraddr,sizeof(svraddr));
	if((svrfd=socket(AF_INET,SOCK_STREAM,0))<0){
		printf_log_c99("debug","%s failed for:%s\n","socket",strerror(errno));
		return 1;
	}
	svraddr.sin_family=AF_INET;
	svraddr.sin_port=htons(port);
	svraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	if((re=bind(svrfd,(struct sockaddr*)(&svraddr),sizeof(svraddr)))<0){
		printf_log_c99("debug","%s failed for:%s\n","bind",strerror(errno));
		return 2;
	}
	if((re=listen(svrfd,BACKLOG))<0){
		printf_log_c99("debug","%s failed for:%s\n","listen",strerror(errno));
		return 3;
	}
	return 0;
}
int socket_bind_listen_epoll_create_epoll_ctl(int &srvfd,int port,int &epollfd,struct epoll_event &ev){
	if(socket_bind_listen(srvfd,port)){
		printf_log_c99("debug","%s failed\n","socket_bind_listen");
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
		printf_log_c99("debug","%s failed for:%s\n","epoll_ctl",strerror(errno));
		return 1;
	}
	return 0;
}
inline int epoll_del(int epollfd,int delfd){
	if(epoll_ctl(epollfd,EPOLL_CTL_DEL,delfd,NULL)<0){
		printf_log_c99("debug","%s failed for:%s\n","epoll_ctl",strerror(errno));
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
int deal_epoll_event(int epollfd,struct epoll_event *resultev,int (*epollin_func)(int),int (*epollout_func)(int)){
	if((resultev->events)&EPOLLIN){
		if(epollin_func !=NULL && epollin_func(resultev->data.fd)){
			printf_log_c99("debug","%s failed\n","epollin_func");
			return 1;
		}
	}else if((resultev->events)&EPOLLOUT){
		if(epollout_func!=NULL && epollout_func(resultev->data.fd)){
			printf_log_c99("debug","%s failed\n","epollout_func");
			return 2;
		}
	}else if((resultev->events)&EPOLLRDHUP){
		if(epoll_del(epollfd,resultev->data.fd)){
			printf_log_c99("debug","%s failed\n","epoll_del");
			return 3;
		}
	}else if((resultev->events)&EPOLLERR){
		printf_log_c99("debug","%s failed for:%s\n","epoll_wait",strerror(errno));
		return 4;
	}else{
		printf_log_c99("debug","%s failed for: unknown event %d\n","deal_epoll_event",resultev->events);
		return 5;
	}
	return 0;
}
