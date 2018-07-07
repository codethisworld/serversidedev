#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <climits>
#include "lhf.h"
#include <netdb.h>
//////////////////////////////////functions/////////////////////////////
void printf_log_low(const char* file,const int line,const char* func,const char* level,const char* format,...){
	va_list va;
	va_start(va,format);
	if(strcmp(level,"debug")==0){
#ifdef DEBUG
		printf("debug ");
		vprintf(format,va);
		printf("\tin %s:%d::%s\n",file,line,func);
#endif//DEBUG
	}else{
		printf("info ");
		vprintf(format,va);
	}
	va_end(va);
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
inline void print_error_inline(int exitstatus,const char* who,const char* why){
	printf_and_exit(exitstatus,"%s:%d::%s\t%s failed for: %s\n",__FILE__,__LINE__,__func__,who,why);
}
void printf_and_exit(const int exitstatus,const char* format,...){
	va_list va;
	va_start(va,format);
	vprintf(format,va);
	va_end(va);
	exit(exitstatus);
}
void hex_dump(void const* vp,size_t n){
	int alignlen=sizeof(int);
	unsigned char const *p=(unsigned char const *)vp;
	for(size_t i=0;i<n;i++){
		printf("%3d ",p[i]);
		if((i%alignlen)==alignlen-1){
			printf("   ");
		}
	}
	putchar('\n');
}
void dump_addrinfo(const char *prefix,struct addrinfo *pai){
	static int calltimes=0;
	char buff[MAXLINE];
	calltimes++;
	printf("%d in %lu address:0x%x\n",calltimes,getpid(),pai);
	printf("%s%-15s %d\n",prefix,"ai_flags",pai->ai_flags);
	printf("%s%-15s %d\n",prefix,"ai_family",pai->ai_family);
	printf("%s%-15s %d\n",prefix,"ai_socktype",pai->ai_socktype);
	printf("%s%-15s %d\n",prefix,"ai_protocol",pai->ai_protocol);
	printf("%s%-15s %d\n",prefix,"ai_addrlen",pai->ai_addrlen);
	printf("%s%-15s %x\n",prefix,"ai_addr",pai->ai_addr);
	snprintf(buff,sizeof(buff),"%s\t",prefix);
	dump_sockaddr(buff,pai->ai_addr);
	printf("%s%-15s %s\n",prefix,"ai_canonname",pai->ai_canonname);
	printf("%s%-15s %x\n\n",prefix,"ai_next",pai->ai_next);
}
void dump_sockaddr(const char *prefix,struct sockaddr *psa){
	static int calltimes=0;
	char buff[MAXLINE];
	calltimes++;
	printf("%d in %lu address:0x%x\n",calltimes,getpid(),psa);
	if(psa->sa_family==AF_INET){
		struct sockaddr_in *psa_in=(struct sockaddr_in*)psa;
		printf("%s%-15s %d(%s)\n",prefix,"sin_family",psa_in->sin_family,"AF_INET");
		printf("%s%-15s %d\n",prefix,"sin_port",ntohs(psa_in->sin_port));
		printf("%s%-15s %s\n\n",prefix,"sin_addr",inet_ntop(AF_INET,&(psa_in->sin_addr),buff,sizeof((psa_in->sin_addr))));
	}else if(psa->sa_family==AF_INET6){
		struct sockaddr_in6 *psa_in=(struct sockaddr_in6*)psa;
		printf("%s%-15s %d(%s)\n",prefix,"sin6_family",psa_in->sin6_family,"AF_INET6");
		printf("%s%-15s %d\n",prefix,"sin6_port",ntohs(psa_in->sin6_port));
		printf("%s%-15s %d(%s)\n",prefix,"sin6_flowinfo",psa_in->sin6_flowinfo);
		printf("%s%-15s %s\n",prefix,"sin6_addr",inet_ntop(AF_INET6,&(psa_in->sin6_addr),buff,sizeof((psa_in->sin6_addr))));
		printf("%s%-15s %d(%s)\n\n",prefix,"sin6_scope_id",psa_in->sin6_scope_id);
	}else{
		printf("sa_family:%d is not AF_INET(%d) and not AF_INET6(%d)\n",psa->sa_family,AF_INET,AF_INET6);
	}
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
	struct addrinfo hints,*paddrinfo,*addrhead;
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
	addrhead=paddrinfo;
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
