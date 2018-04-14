#include <iostream>
#include <string>
#include <map>
using namespace std;
class Test{
public:
	string name;
	Test():name(string("")){};
	Test(const char *_name):name(string(_name)){}
	bool operator <(const Test& ob)const{//both const are needed
		return name.length() < (ob.name.length());
	}
};
void test_struct_key(){
	map<Test,int> maptest;
	map<Test,int>::iterator testiter;
	maptest[Test("111")]=3;
	maptest[Test("11")]=2;
	maptest[Test("1")]=1;
	for(testiter=maptest.begin();testiter!=maptest.end();testiter++){
		cout<<testiter->second<<endl;
	}

}
int main(){
	map<int,string,greater<int> > mapstu;
	mapstu.insert(pair<int,string>(1,"one"));
	mapstu.insert(pair<int,string>(2,"two"));
	mapstu.insert(pair<int,string>(3,"three"));
	mapstu[4]="four";
	mapstu[5]="five";
	mapstu[6]="six";
	map<int,string,greater<int> >::iterator iter;
	for(iter=mapstu.begin();iter!=mapstu.end();iter++){
		cout<<iter->second<<endl;
	}
	test_struct_key();
	return 0;
}
