#ifndef EX6_DESIGN_PATTERNS_REACTOR_H
#define EX6_DESIGN_PATTERNS_REACTOR_H
#endif //EX6_DESIGN_PATTERNS_REACTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>

typedef void (*functype)(int);


typedef struct reactor{
    struct pollfd* pfds;
    functype *moreFuncs;
    pthread_t thread_id;
    int fd_count; // Num of fds
    int fd_size; // size of pollfd
    int listener;
}reactor,*preactor;

void* newReactor(preactor reactor);
void InstallHandler(preactor reactor,void*(func)(void*), int file_des);
void RemoveHandler(preactor reactor,int file_des);
void reactorLoop(reactor *reactor1);

