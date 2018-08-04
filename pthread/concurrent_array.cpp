#include <iostream>
#include <cstring>
#include <ctime>
#include <cassert>
#include <errno.h>
#include <pthread.h>
#include "lhf.h"
using namespace std;

template <class T> 
class concurrent_array{
private:
	T **buff;
	int size;
	int pindex;
	int cindex;
	pthread_mutex_t mutex;
	pthread_cond_t produce_cond;
	pthread_cond_t consume_cond;
public:
	concurrent_array(const int _size):buff(NULL),size(_size),pindex(0),cindex(0){
		buff=new T*[size];
		memset(buff,0,sizeof(buff[0])*size);
		pthread_mutex_init(&mutex,NULL);
		pthread_cond_init(&produce_cond,NULL);
		pthread_cond_init(&consume_cond,NULL);
	}
	~concurrent_array(){
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&produce_cond);
		pthread_cond_destroy(&consume_cond);
		printf("pindex:%d cindex:%d buff[0]:%p\n",pindex,cindex,buff[0]);
		for(int i=cindex;i<=pindex;i=((i+1)%size)){
			if(buff[i]!=NULL){
				cout<<*(buff[i])<<" ";
				delete(buff[i]);
			}
		}
		delete(buff);
	}
	concurrent_array(concurrent_array& ca)=delete;
	concurrent_array& operator=(concurrent_array& ca)=delete;
	inline bool is_full(){
		return (pindex==cindex) && (buff[0]!=NULL);
	}
	inline bool is_empty(){
		return (pindex==cindex) && (buff[0]==NULL);

	}
	int append(T* t){
		int pos;
		if(t==NULL){
			return -1;
		}
		pthread_mutex_lock(&mutex);
		while(is_full()){
			pthread_cond_wait(&consume_cond,&mutex);
		}
		pos=pindex;	
		assert(buff[pos]==NULL);
		assert(t!=NULL);
		buff[pos]=t;
		pindex=(pindex+1)%size;
		pthread_cond_signal(&produce_cond);
		pthread_mutex_unlock(&mutex);
		return pos;
	}
	T* get(){
		T* t;
		pthread_mutex_lock(&mutex);
		while(is_empty()){
			pthread_cond_wait(&produce_cond,&mutex);
		}
		t=buff[cindex];
		buff[cindex]=NULL;
		cindex=(cindex+1)%size;
		pthread_cond_signal(&consume_cond);
		pthread_mutex_unlock(&mutex);
		return t;

	}
};
////////////////////variables////////////////////
static int gproduce_cnt=0;
static int gconsume_cnt=0;

static int gbuff_size=0;
static int gproduce_num=0;
static int gconsume_num=0;
static int gproduct_num=0;
concurrent_array<int> *gca;
////////////////////declerations////////////////////
////////////////////functions////////////////////
void usage(int argc,char* argv[]){
	printf("USAGE: %s <buff_size> <produce_num> <consume_num> <product_num> >& ~/a\n",argv[0]);
	printf("   eg: %s 10 10 10 1000000000\n",argv[0]);
	exit(1);
}
void* produce_thread(void* arg){
	//int index=(int)(long)arg;
	int cnt_cache;
	while((cnt_cache=gproduce_cnt)<gproduct_num){
		bool atomic=__sync_bool_compare_and_swap(&gproduce_cnt,cnt_cache,cnt_cache+1);
		if(atomic){
			int* t=new int(cnt_cache);
			gca->append(t);
		}
	}
	pthread_exit(NULL);
}
void* consume_thread(void* arg){
	int cnt_cache;
	while((cnt_cache=gconsume_cnt)<gproduct_num){
		bool atomic=__sync_bool_compare_and_swap(&gconsume_cnt,cnt_cache,cnt_cache+1);
		if(atomic){
			int *t=gca->get();	
			printf("%20d\n",*t);
			delete(t);
		}
	}
	//sleep(1);
	//assert(gca->is_empty());
	pthread_exit(NULL);
}
int main(int argc,char* argv[]){
	if(argc<5){
		usage(argc,argv);
	}
	//clock_t begin,end,use;
	gbuff_size=atoi(argv[1]);
	gproduce_num=atoi(argv[2]);
	gconsume_num=atoi(argv[3]);
	gproduct_num=atoi(argv[4]);
	int thread_num=gproduce_num+gconsume_num;
	
	//begin=clock();
	gca=new concurrent_array<int>(gbuff_size);
	pthread_t threads[thread_num];
	for(int i=0;i<thread_num;i++){
		void* (*thread_func)(void*) = (i>=gconsume_num)?produce_thread:consume_thread;
		if(pthread_create(threads+i,NULL,thread_func,(void*)(long)i)!=0){
			print_error(1,"pthread_create",strerror(errno));
		}
	}
	for(int i=0;i<thread_num;i++){
		void* exitstatus;
		if(pthread_join(threads[i],&exitstatus)!=0){
			print_error(1,"pthread_create",strerror(errno));
		}
	}
	//end=clock();
	//use=end-begin;
	//printf("buff:%d produce:%d consume:%d product:%d time:%ld(%ld)\n",gbuff_size,gproduce_num,gconsume_num,gproduct_num,use/CLOCKS_PER_SEC,use);
	assert(gca->is_empty());
	return 0;
}
