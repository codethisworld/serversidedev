#include <iostream>
using namespace std;
class Empty{};
class CBox{
	short a;
	int length;
	short d;
	int width;
	int height;
	short b;
	short c;
};
class CBoxStatic{
	int length,width,height;
	static int num;
};
int main(){
	Empty obj;
	CBox box;
	CBoxStatic boxStatic;
	cout<<"size of empty class:"<<sizeof(Empty)<<endl;
	cout<<"size of empty object:"<<sizeof(obj)<<endl;
	cout<<"size of CBox class:"<<sizeof(CBox)<<endl;
	cout<<"size of CBox object:"<<sizeof(box)<<endl;
	cout<<"size of CBoxStatic class:"<<sizeof(CBoxStatic)<<endl;
	cout<<"size of CBoxStatic object:"<<sizeof(boxStatic)<<endl;
	return 0;
}
