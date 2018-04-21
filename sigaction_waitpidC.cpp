//client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <error.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>

void str_cli(FILE *fp, int sfd ) {
	char sendline[1024], recvline[2014];
	memset(recvline, 0, sizeof(sendline));
	memset(sendline, 0, sizeof(recvline));
	while( fgets(sendline, 1024, fp) != NULL ) {
		write(sfd, sendline, strlen(sendline)); 
		if( read(sfd, recvline, 1024) == 0 ) {
			printf("server term prematurely.\n"); 
		}
		fputs(recvline, stdout);
		memset(recvline, 0, sizeof(sendline));
		memset(sendline, 0, sizeof(recvline));
	}
}

int main(){
	int s[5]; 
	for (int i=0; i<5; i++) {
		if( (s[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
			perror("create socket fail.\n");
			exit(0);
		}
	}

	for (int i=0; i<5; i++) {
		struct sockaddr_in server_addr; 
		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(9998);
		inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
		if( connect(s[i], (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 ) {
			perror("connect fail."); 
			exit(0);
		}
	}

	sleep(10);
	str_cli(stdin, s[0]);
	exit(0);
}
