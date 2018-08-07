#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char* argv[]){
	if(argc>1){
		int signo=atoi(argv[1]);
		printf("%2d\t%s\n",signo,strsignal(signo));
	}else{
		for(int i=0;i<40;i++){
			printf("%2d\t%s\n",i,strsignal(i));
		}
	}
	return 0;
}
