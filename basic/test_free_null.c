#include <stdio.h>
#include <stdlib.h>
int main(){
	free(NULL);
	free(0x0);
	return 0;
}
