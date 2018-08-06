#ifndef __socketio_h
#define __socketio_h

////////////////////variables///////////////
enum {MAXFDSIZE=500};
enum {MAX_EVENTS=20};
////////////////////declerations////////////

int accept_epoll_ctl(int epollfd,int srvfd);
int data_echo(int fd);
int deal_epoll_event(int epollfd,struct epoll_event *resultev,int (*epollin_func)(int),int (*epollout_func)(int));
int epoll_add(int epollfd,int addfd,uint32_t events);
inline int epoll_del(int epollfd,int delfd);
int parse_options_common(const char ch,const int optopt,const char* optarg,const int optind,const int opterr);
int read_write(const int fromfd,const int tofd,int& eof);
int read_write_shutdown_if_read_eof(const int fromfd,const int tofd,int& eof);
int socket_connect(int& svrfd,const int port);
int socket_bind_listen(int& svrfd,const int port);
int socket_bind_listen_epoll_create_epoll_ctl(int &srvfd,int port,int &epollfd,struct epoll_event &ev);
#endif //__socketio_h
