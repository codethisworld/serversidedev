#ifndef __LHF_H__
#define __LHF_H__
//////////////////////////////////variables/////////////////////////////
#ifndef DEBUG
	//this variable will not be effective unless is it is set in a file that will be finally included by lhf.cpp 
	//#define DEBUG
#endif//DEBUG
enum {MAXLINE=1024,BACKLOG=1024};
//////////////////////////////////declerations//////////////////////////
#define printf_log_c99(level,format,...) printf_log_low(__FILE__,__LINE__,__func__,level, format,##__VA_ARGS__)
#define printf_log_gcc(level,format,arg...) printf_log_low(__FILE__,__LINE__,__func__,level, format,##arg)
void printf_log_low(const char* file,const int line,const char* func,const char* level,const char* format,...);
//##__VA_ARGS__ vs __VA_ARGS__ : __VA_ARGS__ will failed is ... is empty because of the extra ',' while ##__VA_ARGS__ will remove the extra ',' in this case
#define print_error(A,B,C,...) print_error_low(__FILE__,__LINE__,__func__,A,B,C,##__VA_ARGS__)
void print_error_low(const char* file,const int line,const char* func,int exitstatus,const char* who,const char* why,...);
inline void print_error_inline(int exitstatus,const char* who,const char* why);
void printf_and_exit(const int exitstatus,const char* format,...);
void hex_dump(void const* vp,size_t n);
long strtol_on_error_exit(const char *str,char** pend,const int base,const bool fullmatch);
#endif//__LHF_H__
