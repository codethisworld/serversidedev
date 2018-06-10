#include <sys/socket.h>//recvfrom
#include <netinet/in.h>//struct sockaddr
#include <arpa/inet.h>//htons inet_aton
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "lib/lhf.h"
#include <stdlib.h>
////////////////////variables////////////////////
static int cout=0;
enum {MSG_SIZE=1400};
////////////////////fuctions////////////////////
void print_msg_cout(int signum){
    printf_log_c99("info","server received %d msg\n",cout);
    exit(0);
}
////////////////////main////////////////////
int main(){
    int re;
    int srvfd;
    struct sockaddr_in srvaddr;
    int len=sizeof(srvaddr);
    bzero(&srvaddr,sizeof(srvaddr));
    srvaddr.sin_family=AF_INET;
    srvaddr.sin_port=htons(25000);
    char buff[MSG_SIZE];
    const char* addrstr="127.0.0.1";
    if((re=inet_aton(addrstr,&srvaddr.sin_addr))==0){
        print_error(1,"inet_aton","return 0,means invalid address:%s",addrstr);
    }
    if((srvfd=socket(AF_INET,SOCK_DGRAM,0))<0){
        print_error(1,"socket",strerror(errno));
    }
    if((re=bind(srvfd,(struct sockaddr*)&srvaddr,sizeof(srvaddr)))<0){
        print_error(1,"bind",strerror(errno));
    }
    signal(SIGINT,print_msg_cout);
    while((re=recvfrom(srvfd,buff,sizeof(buff),0,(struct sockaddr*)&srvaddr,(socklen_t*)&len))>=0){
        printf_log_c99("info","recvfrom:%d\n",re);
        cout++;
    }
    print_error(1,"recvfrom",strerror(errno));
    print_msg_cout(SIGINT);
    return 1;
}
