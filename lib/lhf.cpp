#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "lhf.h"
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
void print_error_low(const char* file,const int line,const char* func,int exitstatus,const char* who,const char* why){
	printf_and_exit(exitstatus,"%s:%d::%s\t%s failed for: %s\n",file,line,func,who,why);
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
