#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "lhf.h"
#include "socketio.h"
/////////////////////////////////////////func//////////////////////////////////////////
int accept_epoll_ctl(int epollfd,int srvfd){
	int clifd;
	if((clifd=accept(srvfd,NULL,NULL))<0){
		print_fail_reason("fail","accept",strerror(errno));
		return 1;
	}
	if(epoll_add(epollfd,clifd,EPOLLIN)){
		print_fail("fail");
		return 2;
	}
	return 0;
}
int data_echo(int fd){
	int eof=0;
	int re;
	if((re=read_write_shutdown_if_read_eof(fd,fd,eof))){
		print_fail_reason("fail","read_write_shutdown_if_read_eof","return %d",re);
		return 1;
	}
	return 0;
}
int deal_epoll_event(int epollfd,struct epoll_event *resultev,int (*epollin_func)(int),int (*epollout_func)(int)){
	if((resultev->events)&EPOLLIN){
		if(epollin_func !=NULL && epollin_func(resultev->data.fd)){
			print_fail("fail");
			return 1;
		}
	}else if((resultev->events)&EPOLLOUT){
		if(epollout_func!=NULL && epollout_func(resultev->data.fd)){
			print_fail("fail");
			return 2;
		}
	}else if((resultev->events)&EPOLLRDHUP){
		if(epoll_del(epollfd,resultev->data.fd)){
			print_fail("fail");
			return 3;
		}
	}else if((resultev->events)&EPOLLERR){
		print_fail_reason("fail","epoll_wait",strerror(errno));
		return 4;
	}else{
		print_fail_reason("fail","deal_epoll_event","unknown event %d",resultev->events);
		return 5;
	}
	return 0;
}
int epoll_add(int epollfd,int addfd,uint32_t events){
	struct epoll_event ev;
	int re;
	ev.data.fd=addfd;
	ev.events=events;
	if((re=epoll_ctl(epollfd,EPOLL_CTL_ADD,addfd,&ev))<0){
		print_fail_reason("fail","epoll_ctl",strerror(errno));
		return 1;
	}
	return 0;
}
inline int epoll_del(int epollfd,int delfd){
	if(epoll_ctl(epollfd,EPOLL_CTL_DEL,delfd,NULL)<0){
		print_fail_reason("fail","epoll_ctl",strerror(errno));
		return 1;
	}
	return 0;
}
int parse_options_common(const char ch,const int optopt,const char* optarg,const int optind,const int opterr){
	//debug_enter();
	switch(ch){
		case '?':
			print_fail_reason("fail",__func__,"unknown option:%c",optopt);
			return 1;
		case ':':
			print_fail_reason("fail",__func__,"option %c need value",optopt);
			return 2;
		default:
			print_fail_reason("fail",__func__,"unsolved option %c",ch);
			return 3;
	}
	return 0;
}
int read_write(const int fromfd,const int tofd,int& eof){
	int re;
	char buff[MAXLINE];
	eof=0;
	if((re=read(fromfd,buff,sizeof(buff)))<0){
		print_fail_reason("fail","read",strerror(errno));
		return 1;
	}else if(re==0){//some data may not be received
		//printf_and_exit(0,"%s %s\n",who,exitstr);
		//printf("showdown\n");
		//shutdown(tofd,SHUT_WR);
		eof=1;
		return 0;
	}
	int len=re;
	if(full_write(tofd,buff,len)){
		print_fail("fail");
		return 2;
	}
	return 0;
}
int read_write_shutdown_if_read_eof(const int fromfd,const int tofd,int& eof){
	eof=0;
	if(read_write(fromfd,tofd,eof)){
		print_fail("fail");
		return 1;
	}
	if(eof){
		if(close(fromfd)<0){
			print_fail_reason("fail","close",strerror(errno));
			return 2;
		}
		if(fromfd!=tofd){
			if(close(tofd)<0){
				print_fail_reason("fail","close",strerror(errno));
				return 3;
			}
		}
	}
	return 0;
}
int socket_connect(int& svrfd,const int port){
	struct sockaddr_in svraddr;
	int re;
	if((svrfd=socket(AF_INET,SOCK_STREAM,0))<0){
		print_fail_reason("fail","socket",strerror(errno));
		return 1;
	}
	bzero(&svraddr,sizeof(svraddr));
	svraddr.sin_family=AF_INET;
	svraddr.sin_port=htons(port);
	if((re=connect(svrfd,(struct sockaddr*)(&svraddr),sizeof(svraddr)))<0){
		print_fail_reason("fail","connect",strerror(errno));
		return 2;
	}
	return 0;
}
int socket_bind_listen(int& svrfd,const int port){
	int re;
	struct sockaddr_in svraddr;
	bzero(&svraddr,sizeof(svraddr));
	if((svrfd=socket(AF_INET,SOCK_STREAM,0))<0){
		print_fail_reason("fail","socket",strerror(errno));
		return 1;
	}
	svraddr.sin_family=AF_INET;
	svraddr.sin_port=htons(port);
	svraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	if((re=bind(svrfd,(struct sockaddr*)(&svraddr),sizeof(svraddr)))<0){
		print_fail_reason("fail","bind",strerror(errno));
		return 2;
	}
	if((re=listen(svrfd,BACKLOG))<0){
		print_fail_reason("fail","listen",strerror(errno));
		return 3;
	}
	return 0;
}
int socket_bind_listen_epoll_create_epoll_ctl(int &srvfd,int port,int &epollfd,struct epoll_event &ev){
	if(socket_bind_listen(srvfd,port)){
		print_fail("fail");
		return 1;
	}
	if((epollfd=epoll_create(1))<0){
		print_fail_reason("fail","epoll_create",strerror(errno));
		return 2;
	}
	if(epoll_add(epollfd,srvfd,EPOLLIN)){
		print_fail("fail");
		return 3;
	}
	return 0;
}
