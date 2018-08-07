#include <climits>
#include <errno.h>
#include <fcntl.h>//O_RDONLY
#include <netdb.h>
#include <signal.h>//SIG_IGN
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>//openlog
#include <time.h>
#include <unistd.h>//close getcwd chdir

#include "lhf.h"
//////////////////////////////////functions/////////////////////////////
enum {MAXDESCRIPTOR=64};
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
	printf("%10s %d %d\n","stdin",STDIN_FILENO,fileno(stdin));
	printf("%10s %d %d\n","stdout",STDOUT_FILENO,fileno(stdout));
	printf("%10s %d %d\n","stderr",STDERR_FILENO,fileno(stderr));
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
int date_echo(int fd){
	time_t t=time(NULL);
	char buff[MAXLINE];
	int len=snprintf(buff,sizeof(buff),ctime(&t));//size include '\0',len not include '\0'
	if(full_write(fd,buff,len)){
		printf_log_c99("debug","%s failed\n","full_write");
		return 1;
	}
	return 0;
}
void dump_addrinfo(const char *prefix,struct addrinfo *pai){
	static int calltimes=0;
	char buff[MAXLINE];
	calltimes++;
	printf("%d in %d address:0x%p\n",calltimes,getpid(),pai);
	printf("%s%-15s %d\n",prefix,"ai_flags",pai->ai_flags);
	printf("%s%-15s %d\n",prefix,"ai_family",pai->ai_family);
	printf("%s%-15s %d\n",prefix,"ai_socktype",pai->ai_socktype);
	printf("%s%-15s %d\n",prefix,"ai_protocol",pai->ai_protocol);
	printf("%s%-15s %d\n",prefix,"ai_addrlen",pai->ai_addrlen);
	printf("%s%-15s %p\n",prefix,"ai_addr",pai->ai_addr);
	snprintf(buff,sizeof(buff),"%s\t",prefix);
	dump_sockaddr(buff,pai->ai_addr);
	printf("%s%-15s %s\n",prefix,"ai_canonname",pai->ai_canonname);
	printf("%s%-15s %p\n\n",prefix,"ai_next",pai->ai_next);
}
void dump_sockaddr(const char *prefix,struct sockaddr *psa){
	static int calltimes=0;
	char buff[MAXLINE];
	calltimes++;
	printf("%d in %d address:0x%p\n",calltimes,getpid(),psa);
	if(psa->sa_family==AF_INET){
		struct sockaddr_in *psa_in=(struct sockaddr_in*)psa;
		printf("%s%-15s %d(%s)\n",prefix,"sin_family",psa_in->sin_family,"AF_INET");
		printf("%s%-15s %d\n",prefix,"sin_port",ntohs(psa_in->sin_port));
		printf("%s%-15s %s\n\n",prefix,"sin_addr",inet_ntop(AF_INET,&(psa_in->sin_addr),buff,sizeof((psa_in->sin_addr))));
	}else if(psa->sa_family==AF_INET6){
		struct sockaddr_in6 *psa_in=(struct sockaddr_in6*)psa;
		printf("%s%-15s %d(%s)\n",prefix,"sin6_family",psa_in->sin6_family,"AF_INET6");
		printf("%s%-15s %d\n",prefix,"sin6_port",ntohs(psa_in->sin6_port));
		printf("%s%-15s %u\n",prefix,"sin6_flowinfo",psa_in->sin6_flowinfo);
		printf("%s%-15s %s\n",prefix,"sin6_addr",inet_ntop(AF_INET6,&(psa_in->sin6_addr),buff,sizeof((psa_in->sin6_addr))));
		printf("%s%-15s %u\n\n",prefix,"sin6_scope_id",psa_in->sin6_scope_id);
	}else{
		printf("sa_family:%d is not AF_INET(%d) and not AF_INET6(%d)\n",psa->sa_family,AF_INET,AF_INET6);
	}
}
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
int full_write(int fd,char* buff,int len){
	char* pbuff=buff;
	int re;
	while((re=write(fd,pbuff,len))>0){
		pbuff+=re;
		len-=re;
		assert(len>=0);
		if(len>0){
			printf_log_c99("debug","len:%d write:%d left:%d\n",len+re,re,len);
		}else{
			break;
		}
	}
	if(re<0){
		printf_log_c99("debug","%s failed for:%s\n","write",strerror(errno));
		return 1;
	}
	assert(len==0);
	return 0;	
}
void printf_and_exit(int exitstatus,const char* format,...){
	va_list va;
	va_start(va,format);
	vprintf(format,va);
	va_end(va);
	fflush(stdout);
	fflush(stderr);
	exit(exitstatus);
}
void printf_log_low(const char* file,const int line,const char* func,const char* level,const char* format,...){
	va_list va;
	va_start(va,format);
	if(strcmp(level,"debug")==0){
#ifdef DEBUG
		printf("%s ",level);
		vprintf(format,va);
		printf("\tin %s:%d::%s\n",file,line,func);
#endif//DEBUG
	}else{
		printf("%s ",level);
		vprintf(format,va);
	}
	va_end(va);
}
inline void print_error_inline(int exitstatus,const char* who,const char* why){
	printf_and_exit(exitstatus,"%s:%d::%s\t%s failed for: %s\n",__FILE__,__LINE__,__func__,who,why);
}
void print_error_low(const char* file,const int line,const char* func,int exitstatus,const char* who,const char* why,...){
	int re;
	va_list va;
	va_start(va,why);
	char buff[MAXLINE];
	if((re=vsprintf(buff,why,va))<0){
		perror(buff);
		printf_and_exit(1,"%s failed for %s:%d\n","vsprintf","return value is negative",re);
	}
	va_end(va);
	printf_and_exit(exitstatus,"%s:%d::%s\t%s failed for: %s\n",file,line,func,who,buff);
}
inline void print_error_with_pos(const char* file,int line,const char* func,int exitstatus,const char* who,const char* why){
	printf_and_exit(exitstatus,"%s:%d::%s\t%s failed for:%s\n",file,line,func,who,why);
}
void hex_dump(void const* vp,size_t n){
	unsigned int alignlen=sizeof(int);
	unsigned char const *p=(unsigned char const *)vp;
	for(size_t i=0;i<n;i++){
		printf("%3d ",p[i]);
		if((i%alignlen)==alignlen-1){
			printf("   ");
		}
	}
	putchar('\n');
}
//https://linux.die.net/man/3/strtol
long strtol_on_error_exit(const char *str,char** pend,const int base,const bool fullmatch){
    long re;
    re=strtol(str,pend,base);
    if((errno==ERANGE&&(re==LONG_MAX||re==LONG_MIN))){
        print_error(1,"strtol","%s in base %d is out of range [%ld,%ld]",str,base,LONG_MIN,LONG_MAX);
    }else if((errno!=0 && re==0)){
        print_error(1,"strtol","unknown error");
    }else if(*pend==str){
        print_error(1,"strtol","not fund any digits in base %d in str:\"%s\"",base,str);
    }else if(**pend!='\0'){
        if(fullmatch){
            print_error(1,"strtol","\"%s\" not all in base %d,already parse num:%d remain[%x]:\"%s\" unparsed",str,base,re,*pend,*pend);
        }
    }
    return re;
}
int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp){
	struct addrinfo hints,*paddrinfo;
	int re;
	int srvfd;
	const int on=1;
	bzero(&hints,sizeof(hints));
	hints.ai_flags=AI_PASSIVE;//server side
	hints.ai_family=AF_UNSPEC;//both IPv4 and IPv6 is ok
	hints.ai_socktype=SOCK_STREAM;//tcp
	if((re=getaddrinfo(host,serv,&hints,&paddrinfo))<0){
		print_error(1,"getaddrinfo",gai_strerror(re));
	}
	while(paddrinfo!=NULL){
		dump_addrinfo("",paddrinfo);
		if((re=socket(paddrinfo->ai_family,paddrinfo->ai_socktype,paddrinfo->ai_protocol))<0){
			printf_log_c99("info","%s failed for:%d\n","socket",strerror(errno));
			paddrinfo=paddrinfo->ai_next;
			continue;
		}
		srvfd=re;
		if((re=setsockopt(srvfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0){//note
			printf_log_c99("info","%s failed for:%d\n","setsockopt",strerror(errno));
			paddrinfo=paddrinfo->ai_next;
			close(srvfd);
			continue;
		}
		if((re=bind(srvfd,paddrinfo->ai_addr,paddrinfo->ai_addrlen))<0){
			printf_log_c99("info","%s failed for:%d\n","bind",strerror(errno));
			paddrinfo=paddrinfo->ai_next;
			close(srvfd);
			continue;
		}
		if((re=listen(srvfd,BACKLOG))<0){
			printf_log_c99("info","%s failed for:%d\n","listen",strerror(errno));
			paddrinfo=paddrinfo->ai_next;
			close(srvfd);
			continue;
		}
		*addrlenp=paddrinfo->ai_addrlen;
		break;
	}
	if(paddrinfo==NULL){
		return re;
	}else{
		return srvfd;;
	}
}
