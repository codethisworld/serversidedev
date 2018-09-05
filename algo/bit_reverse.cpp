#include <stdio.h>
#include <stdlib.h>//itoa
unsigned int bit_reverse(unsigned x){
	unsigned re=0;
	int len=sizeof(unsigned)*8;
	for(int fin_bit=0;fin_bit<len;fin_bit++){
		int ori_bit=len-1-fin_bit;
		re|=(((x>>ori_bit))&1)<<fin_bit;
	}
	return re;
}
void usage(int argc,char* argv[]){
	printf("USAGE: %s <num>\n",argv[0]);
	printf("   eg: %s 1\n");
	exit(1);
}
int main(int argc,char* argv[]){
	printf("argc:%d\n",argc);
	if(argc<2){
		usage(argc,argv);	
	}
	unsigned input=atoi(argv[1]);
	unsigned output=bit_reverse(input);
	printf("%x => %x\n",input,output);
	return 0;
}
