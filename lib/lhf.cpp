#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <climits>
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
