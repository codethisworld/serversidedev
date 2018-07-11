#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lhf.h"
////////////////////variables////////////////////
pthread_mutex_t gmutex;
long long gnum=1000;
////////////////////functions////////////////////
/**
condition diff between every case
return *variablep values before the operation:
	if this old_value satisfy the condition,then go on working,local_value=old_value+offset
	otherwise,it is due to the failed conditon check,local_value=old_value
*/
long long ll_gcc_atomic_conditional_change(long long *variablep,long long offset){
	long long old_value;
	bool atomic;
	while((old_value=*variablep)>0){//while satisfy condition,try to change control data and begin to work
		atomic=__sync_bool_compare_and_swap(variablep,old_value,old_value+offset);
		if(atomic){
			//do real work here
			break;
		}
	}
	return old_value;
}
void* work_thread(void* arg){
	long long num_cache=gnum;
	long long current_num;
	long index=(long)arg;
	while(num_cache>0){
		current_num=__sync_val_compare_and_swap(&gnum,num_cache,num_cache-1);
		if(current_num==num_cache){
			num_cache--;
			//printf("%010lld\t%4ld\n",num_cache,index);
		}else{
			num_cache=current_num;
		}
	}
}
////////////////////main////////////////////
int main(int argc,char* argv[]){
	int thread_num=100;
	int re;
	clock_t begin,end,use;
	if(argc>1){
		thread_num=atoi(argv[1]);
	}
	if(argc>2){
		gnum=strtoll(argv[2],NULL,10);
	}
	begin=clock();
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
	end=clock();
	use=end-begin;	
	printf("in main:%d use %d second(%d)\n",gnum,use/CLOCKS_PER_SEC,use);
	return 0;
}
