#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "lhf.h"
////////////////////variables////////////////////
pthread_mutex_t gmutex;
long long gnum=1000;
////////////////////functions////////////////////
void* work_thread(void* arg){
	long long num_cache=gnum;
	long long current_num;
	long index=(long)arg;
	while(num_cache>0){
		current_num=__sync_val_compare_and_swap(&gnum,num_cache,num_cache-1);
		if(current_num==num_cache){
			num_cache--;
			printf("%010lld\t%4ld\n",num_cache,index);
		}else{
			num_cache=current_num;
		}
	}
}
////////////////////main////////////////////
int main(int argc,char* argv[]){
	int thread_num=100;
	int re;
	if(argc>1){
		thread_num=atoi(argv[1]);
	}
	if(argc>2){
		gnum=strtoll(argv[2],NULL,10);
	}
	pthread_t *threads=(pthread_t*)calloc(thread_num,sizeof(pthread_t));
	pthread_mutex_init(&gmutex,NULL);
	for(int i=0;i<thread_num;i++){
		if(pthread_create(threads+i,NULL,work_thread,(void *)(long)i)<0){
			print_error(1,"pthread_create",strerror(errno));
		}
	}
	void *exist_status;
	for(int i=0;i<thread_num;i++){
		if((re=pthread_join(threads[i],&exist_status))<0){
			print_error(1,"pthread_create",strerror(errno));
		}
	}
	pthread_mutex_destroy(&gmutex);
	free(threads);
	printf("in main:%d\n",gnum);
	return 0;
}
