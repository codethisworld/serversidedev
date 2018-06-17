#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>//socket
#include <netinet/in.h>//struct sockaddr
#include <arpa/inet.h>//inet_aton
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "lhf.h"
////////////////////variables////////////////////
////////////////////fucntions////////////////////
void usage(int argc,char* argv[]){
	printf("USAGE:%s domain server_name\n",argv[0]);
	printf("USAGE:%s localhost daytime\n",argv[0]);
	exit(1);	
}
////////////////////main////////////////////
int main(int argc,char* argv[]){
	if(argc<3){
		usage(argc,argv);
	}
	//get h_addr_list
	struct hostent* phe;
	struct in_addr** h_addr_list;
	struct in_addr** paddr;
	struct in_addr addr;
	struct in_addr* addr_list[2];
	if((phe=gethostbyname(argv[1]))==NULL){
		if(inet_aton(argv[1],&addr)==0){
			print_error(1,"inet_aton",strerror(errno));
		}else{
			addr_list[0]=&addr;
			addr_list[1]=NULL;
			h_addr_list=addr_list;
		}

	}else{
		h_addr_list=(struct in_addr**)phe->h_addr_list;
	}

	//get port	
	struct servent* pse;
	if((pse=getservbyname(argv[2],"tcp"))==NULL){
		printf_and_exit(1,"getservbyname failed\n");
	}
	printf("%s:%d\n",argv[2],ntohs(pse->s_port));
	//connect server	
	int srvfd;
	struct sockaddr_in srvaddr;
	
	if((srvfd=socket(AF_INET,SOCK_STREAM,0))<0){
		print_error(1,"socket",strerror(errno));
	}
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_port=pse->s_port;
	for(paddr=h_addr_list;*paddr!=NULL;paddr++){
		memcpy(&(srvaddr.sin_addr),*paddr,sizeof(srvaddr.sin_addr));
		if(connect(srvfd,(struct sockaddr*)&srvaddr,sizeof(srvaddr))<0){
			printf("connect[%s] faile for:%s\n",inet_ntoa(*((struct in_addr*)*paddr)),strerror(errno));
		}else{
			break;
		}
	}
	if(*paddr==NULL){
		printf_and_exit(1,"all conect failed\n");
	}
	//get data
	int re;
	char buff[MAXLINE];
	memset(buff,0,sizeof(buff));
	while((re=read(srvfd,buff,sizeof(buff)))>0){
		puts(buff);
	}
	if(re<0){
		print_error(1,"read",strerror(errno));
	}
	return 0;
}
