#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include "lhf.h"
////////////////////variables////////////////////
////////////////////functions////////////////////
////////////////////main////////////////////
int main(int argc,char* argv[]){
    for(int i=1;i<argc;i++){
        char *name=argv[i];
        struct hostent* phe; 
        char** pstr;
        if((phe = gethostbyname(name) )==NULL){
            print_error(1,"gethostbyname",hstrerror(h_errno));
        }
        printf("h_name\t:%s\n",phe->h_name);
        printf("h_aliases\t:\n");
        for(pstr=phe->h_aliases;*pstr!=NULL;pstr++){
            printf("\t %s\n",*pstr);
        }
        printf("h_length\t:%d\n",phe->h_length);
        printf("h_addr_list\t:\n");
        switch(phe->h_addrtype){
            case AF_INET:
                for(pstr=phe->h_addr_list;*pstr!=NULL;pstr++){
                    char buff[MAXLINE];
                    if(inet_ntop(AF_INET,*pstr,buff,sizeof(buff))==NULL){
                        print_error(1,"inet_ntop",strerror(errno));
                    }
                    printf("\t %s\n",buff);
                }
                break;
            default:
                print_error(1,"gethostbyname","h_addrtype is %d ,not expected value(AF_INET):%d",phe->h_addrtype,AF_INET);
        }
        printf("--------------------------------\n");
    }
    return 0;
}

