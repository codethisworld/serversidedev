#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lhf.h"
#include "socketio.h"
using namespace std;
////////////////////vars////////////////////
enum fd_status{FD_CLOSE=0,FD_RD_OPEN=2^0,FD_WR_OPEN=2^1};
class fd_id{
public:
	int fd;
	int id;
	int status; 
	fd_id():fd(0),id(0),status(FD_CLOSE){

	}
	fd_id(int _fd,int _id):fd(_fd),id(_id),status(FD_CLOSE){

	}
	~fd_id(){

	}
};
int opthread_num=0;
int opconn_num=0;
int opport=0;

int gcnt=0;
int gfinish=0;
int gmsgno=0;
////////////////////func////////////////////
void parse_options(int argc,char* argv[]){
	//debug_enter();
	int re;
	opterr=1;
	char ch;
	while((ch=getopt(argc,argv,"t:c:P:"))!=-1){
		switch(ch){
			case 't':
				opthread_num=atoi(optarg);
				break;
			case 'c':
				opconn_num=atoi(optarg);
				break;
			case 'P':
				opport=atoi(optarg);
				break;
			default:
				if((re=parse_options_common(ch,optopt,optarg,optind,opterr))){
					print_error(1,__func__,"return %d",re);
				}
				break;
		}
	}
}
void sigfunc(int signo){
	printf_log_c99("debug","sigfunc called,gfinish:%d\n",gfinish);
	switch(signo){
		case SIGINT:
			gfinish=1;
			break;
		default:
			break;
	}
}
void* thread_func(void* arg){
	int cnt=0;
	int len=0;
	int eof=0;
	int re;
	fd_id *fds=new fd_id[opconn_num];
	char buff[MAXLINE];
	//debug_enter();
	for(int i=0;i<opconn_num;i++){
		if((re=socket_connect(fds[i].fd,opport))){
			print_fail_reason("fail","socket_connect","return %d",re);
			pthread_exit((void*)(long)1);
		}
		cnt=__sync_fetch_and_add(&gcnt,1);
		fds[i].id=cnt;
		fds[i].status=FD_RD_OPEN|FD_WR_OPEN;
	}
	printf_log_c99("debug","socket_connect ok,cnt:%d\n",cnt);
	for(int i=0,finishcnt=0;finishcnt<opconn_num;i=(i+1)%opconn_num){
		int msgno;
		if(fds[i].status == FD_CLOSE){
			continue;
		}
		if(gfinish){
			printf_log_c99("debug","sent shutdown,cnt:%d\n",fds[i].id);
			if(shutdown(fds[i].fd,SHUT_WR)<0){
				print_error(1,"shutdown",strerror(errno));
			}
			fds[i].status=FD_RD_OPEN;
		}else{
			len=0;
			msgno=gmsgno++;
			do{
				len=snprintf(buff,sizeof(buff),"%d %d %d\n",fds[i].id,fds[i].fd,msgno);
			}while(len<0);
			if(full_write(fds[i].fd,buff,len)<0){
				print_error(1,"close",strerror(errno));
			}
			printf_log_c99("debug","write ok,buff:%s\n",buff);
		}
		eof=0;
		if((re=read_write(fds[i].fd,fileno(stdout),eof))){
			print_fail_reason("fail","read_write","return %d",re);
		}
		printf_log_c99("debug","read_write ok,t:%d c:%d P:%d i:%d fd:%d id:%d msgno:%d\n",opthread_num,opconn_num,opport,i,fds[i].fd,fds[i].id,msgno);
		if(eof){
			fds[i].status=FD_CLOSE;
			finishcnt++;
			close(fds[i].fd);
		}
		if(i+1==opconn_num){
			sleep(1);
		}
	}
	
	printf_log_c99("debug","all fd closed\n");
	delete[] fds;
	pthread_exit(NULL);
}
////////////////////main////////////////////
int main(int argc,char* argv[]){
	parse_options(argc,argv);
	printf_log_c99("debug","parse_options ok,t:%d c:%d P:%d\n",opthread_num,opconn_num,opport);
	signal(SIGINT,sigfunc);
	printf_log_c99("debug","signal ok\n");
	pthread_t *threads=new pthread_t[opthread_num];
	for(int i=0;i<opthread_num;i++){
		if(pthread_create(threads+i,NULL,thread_func,NULL)!=0){
			print_error(1,"pthread_create",strerror(errno));
		}
	}
	printf_log_c99("debug","pthread_create ok\n");
	for(int i=0;i<opthread_num;i++){
		void *exitstatus;
		if(pthread_join(threads[i],&exitstatus)!=0){
			print_error(1,"pthread_join",strerror(errno));
		}
		printf_log_c99("debug","thread %d exit with value:%d\n",i,(int)(long)exitstatus);
	}
	printf_log_c99("debug","pthread_join ok\n");
	delete[] threads;
	fflush(stdout);
	fflush(stderr);
	return 0;
}
