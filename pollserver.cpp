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
#include "reactor.hpp"
#include "Pipeline.h"

#define PORT "9034"   // Port we're listening on

//global reactor
struct reactor* r1 = NULL;
void receive(int fd);

void initReactor(int max_size) {
    printf("Try to create reactor..\n");
    r1=(struct reactor*)malloc(sizeof(struct reactor));
    r1->fd_size=max_size;
    r1->fd_count=0;
    r1->pfds=(pollfd*)malloc(sizeof (*r1->pfds) * r1->fd_size);
    r1->moreFuncs= (void (**)(int))malloc(r1->fd_size * sizeof(r1->moreFuncs));
}

// Return a listening socket
int get_listener_socket(void){
    int listener;     // Listening socket descriptor
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int rv;
    struct addrinfo hints, *ai, *p;
    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        break;
    }

    freeaddrinfo(ai); // All done with this

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }
    return listener;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
 * This func will listen for new clients that want to connect to the server
 */
void listenForConnections(int listener) {
    printf("new connection..\n");
    int newfd;        // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;
    char remoteIP[INET6_ADDRSTRLEN];
    addrlen = sizeof remoteaddr;
    newfd = accept(listener, (struct sockaddr *) &remoteaddr, &addrlen);

    if (newfd == -1) {
        perror("accept");
    } else {
        InstallHandler(r1,(void* (*)(void*))receive,newfd);
        printf("pollserver: new connection from %s on "
               "socket %d\n",
               inet_ntop(remoteaddr.ss_family,
                         get_in_addr((struct sockaddr *) &remoteaddr),
                         remoteIP, INET6_ADDRSTRLEN),
               newfd);

    }
}
void receive(int fd){
    printf("client send data\n");
    char buf[256];
    int nbytes = recv(fd, buf, sizeof buf, 0);

    int sender_fd = fd;
    if (nbytes <= 0) {
        fflush(stdout);
        // Got error or connection closed by client
        if (nbytes == 0) {
            // Connection closed
            printf("pollserver: socket %d hung up\n", sender_fd);
        } else {
            perror("recv");
        }
        close(fd); // Bye!
        RemoveHandler(r1,fd);

    } else {
        fflush(stdout);
        // We got some good data from a client
        for (int j = 0; j < r1->fd_count; j++) {

            // Send to everyone!
            int dest_fd = r1->pfds[j].fd;
            
            fflush(stdout);
            // Except the listener and ourselves
            if (dest_fd != r1->listener && dest_fd != sender_fd) {
                Node *node = newNode(buf, dest_fd);
                enQ(q1, node);
            }
        }
        bzero(buf, 256);
    }
}

int main(int argc, char const *argv[]){
    
    if (argc != 2)
    {
        exit(1);
    }
    int max_num = atoi(argv[1]);
    if (max_num < 2 || max_num > 10)
    {
        perror("numberof clients should be between 2-10");
        exit(1);
    }
    initReactor(max_num);

    q1 = createQ();
    q2 = createQ();
    q3 = createQ();
    q4 = createQ();

    Pipeline *pipe = createPipeline(q1, q2, q3, q4, removeSpecialChars, toUpper, isPalindrom, sendNode);
    int listener;
    
    listener = get_listener_socket();
    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }
    InstallHandler(r1,(void* (*)(void*))listenForConnections,listener); // install the listener in the pollfds of r1
    r1= (preactor) newReactor(r1); // active the reactor's main thread
    return 0;
}
