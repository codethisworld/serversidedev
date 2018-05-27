#ifndef __LHF_H__
#define __LHF_H__
//////////////////////////////////variables/////////////////////////////
//////////////////////////////////declerations//////////////////////////
#define printf_log_c99(level,format,...) printf_log_low(__FILE__,__LINE__,__func__,level, format,__VA_ARGS__);
#define printf_log_gcc(level,format,arg...) printf_log_low(__FILE__,__LINE__,__func__,level, format,##arg);
void printf_log_low(const char* file,const int line,const char* func,const char* level,const char* format,...);

#endif//__LHF_H__
