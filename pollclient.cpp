#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <arpa/inet.h>



#define TRUE 1
const int MAX_CMD_SIZE = 1024;
#define SERVER_PORT 9034
#define SERVER_IP_ADDRESS "127.0.0.1"
int sockD=-1;
int isConnected=0;

void sendMSG(){
    char newMSG[MAX_CMD_SIZE] = {0};
    while (isConnected != 0)
    {
        fgets(newMSG,MAX_CMD_SIZE,stdin);
        newMSG[strlen(newMSG) - 1] = '\0';
        printf("Sender: %s\n",newMSG);
        if (strncmp(newMSG,"EXIT",4) == 0)
        {
            send(sockD,"EXIT",4,0);
            isConnected = 0;
            break;
        }
        if (send(sockD, newMSG, strlen(newMSG), 0) == -1)
        {
            printf("%d\n",strlen(newMSG));
            perror("send");
        }
        bzero(newMSG, MAX_CMD_SIZE);
    }
}

void recvMSG(){

    char serverData[MAX_CMD_SIZE] = {0};
    isConnected = 1;
    int numOfBytes = 0;
    while ((numOfBytes = recv(sockD, serverData, MAX_CMD_SIZE, 0)) != -1){
        if (!numOfBytes){
            isConnected = 0;
            break;
        }
        printf("Received: %s\n",serverData);
        if (!strcmp(serverData, "EXIT"))
        {
            isConnected = 0;
            break;
        }
        bzero(serverData, MAX_CMD_SIZE);
    }


//    while (TRUE){
//        char serverData[1024];
//        int numOfBytes;
//        numOfBytes = recv(sock, serverData, 1024, 0);
//        serverData[numOfBytes] = '\0';
////        if (strncmp(serverData,"EXIT",4) == 0) {
////            printf("EXIT was entered by client.. bye bye\n");
////            break;
////        }
//        printf("Received: %s\n",serverData);
//    }
//    printf("reciveMSG\n");
//    char serverData[1024];
//    int numOfBytes;
//    while (TRUE){
//        while ((numOfBytes = recv(sock, serverData, 1024, 0)) != -1){
//            printf("got data\n");
//            serverData[numOfBytes] = '\0';
//            printf("Received: %s\n",serverData);
//            bzero(serverData, 1024);
//        }
//    }
//
//    printf("i am here :(\n");
}


int main(){

    sockD = socket(AF_INET, SOCK_STREAM, 0);
    if (sockD == -1)
    {
        perror("socket");
        return -1;
    }

    signal(SIGPIPE, SIG_IGN); // on linux to prevent crash on closing socket

    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port= htons(SERVER_PORT);
    servAddr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);;

    int connectStatus = connect(sockD, (struct sockaddr*)&servAddr,sizeof(servAddr));

    if (connectStatus == -1) {
        printf("Error...\n");
        return 0;
    }
    printf("A new client has established a connection with the server\n");

    pthread_t th[2];
    isConnected=1;
    if (pthread_create(&th[0], NULL, (void* (*)(void*))sendMSG, NULL) != 0) {
        perror("Failed to create thread");
    }
    if (pthread_create(&th[1], NULL, (void* (*)(void*))recvMSG, NULL) != 0) {
        perror("Failed to create thread");
    }
    if (pthread_join(th[0], NULL) != 0) {
        perror("Failed to join thread");
    }
    pthread_cancel(th[1]);
    close(sockD);


    return 0;
}
