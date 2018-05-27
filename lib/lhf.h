#ifndef __LHF_H__
#define __LHF_H__
//////////////////////////////////variables/////////////////////////////
#ifndef DEBUG
	//this variable will not be effective unless is it is set in a file that will be finally included by lhf.cpp 
	//#define DEBUG
#endif//DEBUG
enum {MAXLINE=1024};
//////////////////////////////////declerations//////////////////////////
#define printf_log_c99(level,format,...) printf_log_low(__FILE__,__LINE__,__func__,level, format,__VA_ARGS__);
#define printf_log_gcc(level,format,arg...) printf_log_low(__FILE__,__LINE__,__func__,level, format,##arg);
void printf_log_low(const char* file,const int line,const char* func,const char* level,const char* format,...);

#define print_error(A,B,C) print_error_low(__FILE__,__LINE__,__func__,A,B,C)
void print_error_low(const char* file,const int line,const char* func,int exitstatus,const char* who,const char* why);
inline void print_error_inline(int exitstatus,const char* who,const char* why);
void printf_and_exit(const int exitstatus,const char* format,...);
#endif//__LHF_H__
