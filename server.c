/*
** server.c -- a stream socket server demo
We used the code at the following link:
https://www.geeksforgeeks.org/handling-multiple-clients-on-server-with-multithreading-using-socket-programming-in-c-cpp/
*/

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h> 
#include <stdbool.h>
#include "Queue.h"
#include "ActiveObject.h"
#include "Pipeline.h"


void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

pthread_t thread[100];

void* threads_handler(void* new_fd){

    printf("***  threads handler func  ***\n");

    int fd = *(int*)new_fd;
    
    char input[1024];
    int len = 0;
    
    // char* output;

    while(1) {

		if((len = recv(fd, input, 1024, 0)) == -1){
  
            perror("recv");
            exit(1);
        } 

        if(len > 0){

            input[len] = '\0';

            printf("recv: %s\n", input);

            Node* node = newNode(input, fd);

            enQ(q1, node);
        }

        memset(input, 0, sizeof(input));
    }

}

int main(void){

    q1 = createQ();
    q2 = createQ();
    q3 = createQ();
    q4 = createQ();

    Pipeline* pipe = createPipeline(q1, q2, q3, q4, removeSpecialChars, toUpper,isPalindrom, sendNode);

    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    int i = 0;
    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        
        sin_size = sizeof their_addr;
        
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        // int *new_sock = malloc(sizeof (int));
        // *new_sock = new_fd;
        
        if (pthread_create(&thread[i++], NULL,&threads_handler, &new_fd)!= 0){
            printf("Failed to create thread\n");
            close(new_fd);
            // free(new_sock);
            exit(0);
        }
    }

    //  pthread_mutex_destroy(&lock);

    return 0;
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

        errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
