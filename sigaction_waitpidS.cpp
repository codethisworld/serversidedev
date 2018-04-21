//server.c
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
#include <sys/wait.h>

typedef void sigfunc(int);

void func_wait(int signo) {
	if(signo==SIGCHLD){
		pid_t pid;
		int stat;
		pid = wait(&stat);    
		printf( "child %d exit\n", pid );
	}
	return;
}
void func_waitpid(int signo) {
	if(signo==SIGCHLD){
		pid_t pid;
		int stat;
		while( (pid = waitpid(-1, &stat, WNOHANG)) > 0 ) {
			printf( "child %d exit\n", pid );
		}
		if(pid<0){
			printf("%d : %s\n",pid,strerror(errno));
		}
	}
	return;
}

sigfunc* signal( int signo, sigfunc *func ) {
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if ( signo == SIGALRM ) {
#ifdef            SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;    /* SunOS 4.x */
#endif
	} else {
#ifdef           SA_RESTART
		act.sa_flags |= SA_RESTART;    /* SVR4, 4.4BSD */
#endif
	}
	if ( sigaction(signo, &act, &oact) < 0 ) {
		return SIG_ERR;
	}
	return oact.sa_handler;
} 


void str_echo( int cfd ) {
	ssize_t n;
	char buf[1024];
again:
	memset(buf, 0, sizeof(buf));
	while( (n = read(cfd, buf, 1024)) > 0 ) {
		write(cfd, buf, n); 
	}
	if( n <0 && errno == EINTR ) {
		goto again; 
	} else {
		printf("str_echo: read error\n");
	}
}

int main() {

	signal(SIGCHLD, &func_waitpid);    

	int s, c;
	pid_t child;
	if( (s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		perror("create socket fail.\n");
		exit(0);
	}

	struct sockaddr_in server_addr, child_addr; 
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9998);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if( bind(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 ) {
		perror("bind address fail.\n");
		exit(0);
	}

	if( listen(s, 1024) < 0 ) {
		perror("listen fail.\n");
		exit(0);
	}
	while(1) {
		socklen_t chilen = sizeof(child_addr); 
		if ( (c = accept(s, (struct sockaddr *)&child_addr, &chilen)) < 0 ) {
			perror("listen fail.");
			exit(0);
		}

		if( (child = fork()) == 0 ) {
			close(s); 
			str_echo(c);
			exit(0);
		}
		close(c);
	}
}
