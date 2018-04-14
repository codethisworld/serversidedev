#include <iostream>
#include <string.h>
using namespace std;
class mystring{
public:
	mystring(){
		cout<<"constructor"<<endl;
		data=NULL;
	};
	mystring(const char* _data){
		cout<<"constructor"<<endl;
		data=NULL;
		int len=strlen(_data);
		if(len>0){
			data=new char[len+1];
			data[0]='\0';
			strcat(data,_data);
		}
	}
	virtual ~mystring(){
		cout<<"destructor"<<endl;
		if(data!=NULL){
			delete data;
			data=NULL;
		}
	}
	unsigned length() const{//const member function
		return strlen(data);
	}
	mystring(const mystring& in){
		cout<<"copy constructor"<<endl;
		int len=in.length();
		if(len>0){
			data=new char[len+1];
			data[0]='\0';
			strcat(data,in.data);
		}
	}
	mystring& operator=(const mystring& in){
		cout<<"assign operator"<<endl;
		if(data!=NULL){
			delete data;
			data=NULL;
		}
		int len=in.length();
		if(len>0){
			data=new char[len+1];
			data[0]='\0';
			strcat(data,in.data);
		}
		return *this;
	}
	bool operator==(const mystring& in){
		cout<<"equal operator"<<endl;
		return strcmp(data,in.data)==0;
	}
	mystring operator+(const mystring& in){
		mystring re;
		if(data==NULL){
			re=in;
		}else if(in.data==NULL){
			re=*this;
		}else{
			int len=length()+in.length();
			re.data=new char[len+1];
			strcpy(re.data,data);
			strcat(re.data,in.data);
		}
		cout<<"end of plus operator"<<endl;
		return re;
	}
	friend ostream& operator<<(ostream& out, mystring& str){//not in this class
		out<<str.data;
		return out;
	}
	
private:
	char* data;
};
int main(){
	mystring str1=mystring("one");
	mystring str2=mystring("two");
	mystring str3=str1+str2;
	cout<<"first plus operator okay"<<endl;	
	mystring str4=str1+mystring();
	cout<<"str1:"<<str1<<endl;	
	cout<<"str2:"<<str2<<endl;	
	cout<<"str3:"<<str3<<endl;	
	if(str3==str1){
		cout<<"str3==str1 true"<<endl;
	}else{
		cout<<"str3==str1 false"<<endl;
	}
	cout<<"str4:"<<str4<<endl;	
	return 0;
}
