#ifndef __LHF_H__
#define __LHF_H__
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
//////////////////////////////////variables/////////////////////////////
#ifndef DEBUG
	//this variable will not be effective unless is it is set in a file that will be finally included by lhf.cpp 
	//#define DEBUG
#endif//DEBUG
enum {MAXLINE=1024,BACKLOG=1024};
//////////////////////////////////declerations//////////////////////////
#define printf_log_c99(level,format,...) printf_log_low(__FILE__,__LINE__,__func__,level, format,##__VA_ARGS__)
#define printf_log_gcc(level,format,arg...) printf_log_low(__FILE__,__LINE__,__func__,level, format,##arg)
//##__VA_ARGS__ vs __VA_ARGS__ : __VA_ARGS__ will failed is ... is empty because of the extra ',' while ##__VA_ARGS__ will remove the extra ',' in this case
#define print_error(A,B,C,...) print_error_low(__FILE__,__LINE__,__func__,A,B,C,##__VA_ARGS__)
#define print_fail(level) do{printf_log_c99(level,"%s failed\n",__func__);}while(0)
#define print_fail_reason(level,who,why,...) do{char buff[MAXLINE];snprintf(buff,sizeof(buff),why,##__VA_ARGS__);printf_log_c99(level,"%s failed for:%s\n",who,buff);}while(0)
#define debug_enter() do{printf_log_c99("debug","enter %s\n",__func__);}while(0);
#define debug_leave() do{printf_log_c99("debug","leave %s\n",__func__);}while(0);

int daemon_init(const char *pname,int facility);
int date_echo(int fd);
void dump_addrinfo(const char *prefix,struct addrinfo *pai);
void dump_sockaddr(const char *prefix,struct sockaddr *psa);
void fork_and_reserve_child_process();
int full_write(int fd,char* buff,int len);
void printf_and_exit(const int exitstatus,const char* format,...);
void printf_log_low(const char* file,const int line,const char* func,const char* level,const char* format,...);
inline void print_error_inline(int exitstatus,const char* who,const char* why);
void print_error_low(const char* file,const int line,const char* func,int exitstatus,const char* who,const char* why,...);
void print_error_with_pos(const char* file,int line,const char* func,int exitstatus,const char* who,const char* why);
void hex_dump(void const* vp,size_t n);
long strtol_on_error_exit(const char *str,char** pend,const int base,const bool fullmatch);
int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp);


#endif//__LHF_H__
