#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
////////////////////variables////////////////////
int gline_output_num=20;
////////////////////declerations////////////////////
inline void swap(int *array,int a,int b);
int partition(int *array,int in_left,int in_right);
void print_array(int *array,int left,int right);
void qsort(int *array,int in_left,int in_right);
int is_increase(int* array,int left,int right);
////////////////////functions////////////////////
inline void swap(int *array,int a,int b){
	int tmp=array[a];
	array[a]=array[b];
	array[b]=tmp;
}
int partition(int *array,int in_left,int in_right){
	int left=in_left;
	int right=in_right;
	assert(right>=left);
	int pivot=array[(left+right)/2];
	int re;
	while(left<right){
		if(array[left]<=pivot){
			left++;
		}else if(array[right]>=pivot){
			right--;
		}else{
			swap(array,left,right);
		}
	}
	assert(left==right);
	if(array[left]>pivot){
		re=left-1;
	}else if(array[left]<pivot){
		re=left+1;	
	}else{
		re=left;
	}
	if(array[re]!=pivot){
		print_array(array,in_left,in_right);	
		printf("left:%d right:%d re:%d array[re]:%d pivot:%d\n",in_left,in_right,re,array[re],pivot);
	}
	assert(array[re]==pivot);
	return re;
}
void print_array(int *array,int left,int right){
	for(int i=left;i<=right;i++){
		printf("%d ",array[i]);
		if(i%gline_output_num==0){
			printf("\n");
		}
	}
	printf("\n");
}
void qsort(int *array,int in_left,int in_right){
	int left=in_left;
	int right=in_right;
	assert(right>=left);
	if(left==right){
		return;
	}else if(left==right-1){
		if(array[left]>array[right]){
			swap(array,left,right);
		}
	}else{//right-left>=2,least 3 num
		int index=partition(array,left,right);
		qsort(array,left,index-1);
		qsort(array,index+1,right);
	}
}
int is_increase(int* array,int left,int right){
	for(int i=left;i<right;i++){
		if(array[i]>array[i+1]){
			return 0;
		}
	}
	return 1;
}
////////////////////main////////////////////
int main(int argc,char* argv[]){
	int num=100;
	clock_t begin,end,use;
	if(argc>1){
		num=atoi(argv[1]);
	}
	if(argc>2){
		gline_output_num=atoi(argv[2]);
	}
	int *array=(int *)calloc(num,sizeof(int));
	srand(time(NULL));
	for(int i=0;i<num;i++){
		array[i]=rand();
	}
	begin=clock();
	qsort(array,0,num-1);
	end=clock();
	use=end-begin;
	printf("%d num use %ld second(%ld)",num,use/CLOCKS_PER_SEC,use);
	if(is_increase(array,0,num-1)){
		printf("sort ok\n");
	}else{
		printf("sort fail\n");
	}
	free(array);
	return 0;
}
