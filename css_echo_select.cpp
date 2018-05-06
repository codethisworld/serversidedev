#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>//bind socket
#include <netinet/in.h>//struct sockaddr_in
#include <arpa/inet.h>//htons
#include <stdarg.h>//va_list
#include <string.h>//strerror
#include <errno.h>//errno
#include <sys/select.h>//select
#include <sys/time.h>//timeval
#include <unistd.h>//write close
/////////////////////////////////////////variables/////////////////////////////////////
enum {MAXFDSIZE=500,BACKLOG=1000,MAXLINE=1024};
/////////////////////////////////////////func//////////////////////////////////////////
#define print_error(A,B,C) print_error_with_pos(__FILE__,__LINE__,__func__,A,B,C)
void printf_and_exit(int exitstatus,const char* format,...){
	va_list va;
	va_start(va,format);
	vprintf(format,va);
	va_end(va);
	exit(exitstatus);
}
inline void print_error_with_pos(const char* file,int line,const char* func,int exitstatus,const char* who,const char* why){
	printf_and_exit(exitstatus,"%s:%d::%s\t%s failed for:%d\n",file,line,func,who,why);
}
void socket_bind_listen(int& svrfd,const int port){
	int re;
	struct sockaddr_in svraddr;
	bzero(&svraddr,sizeof(svraddr));
	if((svrfd=socket(AF_INET,SOCK_STREAM,0))<0){
		print_error(1,"socket",strerror(errno));
	}
	svraddr.sin_family=AF_INET;
	svraddr.sin_port=htons(port);
	svraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	if((re=bind(svrfd,(struct sockaddr*)(&svraddr),sizeof(svraddr)))<0){
		print_error(1,"bind",strerror(errno));
	}
	if((re=listen(svrfd,BACKLOG))<0){
		print_error(1,"listen",strerror(errno));
	}
}
/////////////////////////////////////////main//////////////////////////////////////////
int main(){
	int svrfd;
	int re;
	fd_set allset;//all fd that need to be test
	fd_set rset;//fd_set used to call select,may be changed
	int maxfd;//used for maxfdp1
	int clifds[MAXFDSIZE];//client fd array
	int maxi;//max index of client fd array,used for test result loop
	socket_bind_listen(svrfd,25000);

	FD_ZERO(&allset);
	FD_SET(svrfd,&allset);
	maxfd=svrfd;

	for(int i=0;i<MAXFDSIZE;i++){
		clifds[i]=-1;
	}
	maxi=-1;

	while(1){
		rset=allset;
		if((re=select(maxfd+1,&rset,NULL,NULL,NULL))<0){
			print_error(1,"select",strerror(errno));
		}else if(re==0){
			continue;
		}
		int fdnum=re;
		if(FD_ISSET(svrfd,&rset)){
			if((re=accept(svrfd,NULL,NULL))<0){
				print_error(1,"accept",strerror(errno));
			}
			int clifd=re;
			FD_SET(clifd,&allset);
			if(clifd>maxfd){
				maxfd=clifd;
			}
			int i;
			for(i=0;i<MAXFDSIZE;i++){
				if(clifds[i]==-1){
					clifds[i]=clifd;
					break;
				}
			}
			if(i==MAXFDSIZE){
				printf("only %d connect allowed,to much connections\n",MAXFDSIZE);
			}else if(i>maxi){
				maxi=i;
			}
			if(--fdnum<=0){
				continue;
			}
		}
		for(int i=0;i<=maxi;i++){
			char buff[MAXLINE];
			if(FD_ISSET(clifds[i],&rset)){
				if((re=read(clifds[i],buff,MAXLINE))<0){
					print_error(1,"read",strerror(errno));
				}
				if(re==0){
					printf("connect %d closed\n",clifds[i]);
					close(clifds[i]);
					FD_CLR(clifds[i],&allset);
					if(clifds[i]==maxfd){
						maxfd--;
					}
					clifds[i]=-1;
					continue;
					//if we decrease maxi here,then some fd will not be checked.we should do it after all fds are checked
				}
				int len=re;
				if((re=write(clifds[i],buff,len))<0){
					print_error(1,"write",strerror(errno));
				}
				if(--fdnum<=0){
					break;
				}
			}
		}
		for(int i=maxi;i>=0 && clifds[i]==-1;i--){
			maxi--;
		}
	}
}
