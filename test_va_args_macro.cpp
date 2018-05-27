#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "lib/lhf.h"
//////////////////////////////////variables/////////////////////////////
#ifndef DEBUG
	#define DEBUG
#endif//DEBUG

//////////////////////////////////main//////////////////////////////////
int main(){
	printf_log_c99("debug","va_args_macro using %s %s level info\n","c99","with");
	printf_log_c99("","va_args_macro using %s %s level info\n","c99","without");
	printf_log_c99("a","va_args_macro using %s %s level info\n","c99","without");
	printf_log_gcc("debug","va_args_macro using %s %s level info\n","gcc","with");
	printf_log_gcc("","va_args_macro using %s %s level info\n","gcc","without");
	printf_log_gcc("a","va_args_macro using %s %s level info\n","gcc","without");
	return 0;
}
