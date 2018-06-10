#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>//sento
#include <netinet/in.h>//struct sockaddr
#include <arpa/inet.h>//htons inet_aton
#include <string.h>
#include <errno.h>
#include "lib/lhf.h"
////////////////////variables////////////////////
enum {MSG_NUM=1000,MSG_SIZE=1400};
////////////////////fuctions////////////////////
////////////////////main////////////////////
int main(){
    int re;
    int srvfd;
    if((srvfd=socket(AF_INET,SOCK_DGRAM,0))<0){
        print_error(1,"socket",strerror(errno));
    }
    char buff[MSG_SIZE];
    struct sockaddr_in srvaddr;
    int len=sizeof(srvaddr);
    bzero(&srvaddr,sizeof(srvaddr));
    srvaddr.sin_family=AF_INET;
    srvaddr.sin_port=htons(25000);
    const char* addrstr="127.0.0.1";
    if((re=inet_aton(addrstr,&srvaddr.sin_addr))==0){
        print_error(1,"inet_aton","return 0,means invalid address:%s",addrstr);
    }
    for(int i=0;i<MSG_NUM;i++){
        if((re=sendto(srvfd,buff,sizeof(buff),0,(struct sockaddr*)&srvaddr,len))<0){
            print_error(1,"sendto",strerror(errno));
        }
    }
    printf_log_c99("info","sentto:%d\n",re);
    printf_log_c99("info","%d msg have been sent!\n",MSG_NUM);
    return 0;
}
