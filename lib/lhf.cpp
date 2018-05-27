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
		printf("debug %s:%d::%s\t",file,line,func);
		vprintf(format,va);
#endif//DEBUG
	}else{
		printf("info  %s:%d::%s\t",file,line,func);
		vprintf(format,va);
	}
	va_end(va);
}
