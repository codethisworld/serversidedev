#ifndef __socketio_h
#define __socketio_h

////////////////////variables///////////////
enum {MAXFDSIZE=500};
enum {MAX_EVENTS=20};
////////////////////declerations////////////

int accept_epoll_ctl(int epollfd,int srvfd);
int deal_epoll_event(int epollfd,struct epoll_event *resultev,int (*epollin_func)(int),int (*epollout_func)(int));
int epoll_add(int epollfd,int addfd,uint32_t events);
inline int epoll_del(int epollfd,int delfd);
void printf_and_exit(int exitstatus,const char* format,...);
inline void print_error_with_pos(const char* file,int line,const char* func,int exitstatus,const char* who,const char* why);
int socket_bind_listen(int& svrfd,const int port);
int socket_bind_listen_epoll_create_epoll_ctl(int &srvfd,int port,int &epollfd,struct epoll_event &ev);
#endif //__socketio_h
