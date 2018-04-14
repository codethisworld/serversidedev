#include <iostream>
#include <stdlib.h>//qsort
#define MAX_LEN 5
using namespace std;
int cmp(const void *a,const void *b){
	return *(int*)a-*(int*)b;
}
template <class T>
class PrintArray{
public:
	void print(T *array,unsigned len){
		for(int i=0;i<len;i++){
			cout<<array[i]<<" ";
		}
		cout<<endl;
	}
};
int main(){
	int array[MAX_LEN];
	double doublearray[MAX_LEN];
	int n=0;
	PrintArray<int> intprinter;
	PrintArray<double> doubleprinter;
	for(;n<MAX_LEN && cin>>array[n];n++);
	intprinter.print(array,n);
	qsort(array,n,sizeof(int),cmp);
	intprinter.print(array,n);
	cout<<endl;

	n=0;
	for(;n<MAX_LEN && cin>>doublearray[n];n++);
	doubleprinter.print(doublearray,n);
	qsort(doublearray,n,sizeof(double),cmp);
	doubleprinter.print(doublearray,n);
	cout<<endl;
	return 0;
}
