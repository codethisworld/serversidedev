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

int daemon_init(const char *pname,int facility);
int date_echo(int fd);
void dump_addrinfo(const char *prefix,struct addrinfo *pai);
void dump_sockaddr(const char *prefix,struct sockaddr *psa);
int full_write(int fd,char* buff,int len);
void hex_dump(void const* vp,size_t n);
void printf_and_exit(const int exitstatus,const char* format,...);
void printf_log_low(const char* file,const int line,const char* func,const char* level,const char* format,...);
void print_error_low(const char* file,const int line,const char* func,int exitstatus,const char* who,const char* why,...);
inline void print_error_inline(int exitstatus,const char* who,const char* why);
long strtol_on_error_exit(const char *str,char** pend,const int base,const bool fullmatch);
int tcp_listen(const char *host, const char *serv, socklen_t *addrlenp);


#endif//__LHF_H__
