
# pragma once 

#include <pthread.h>
#include <ctype.h>
#include "Queue.h"

// Active Object:
typedef struct ActiveObject {
    
    Queue *queue;
    void (*firstFunc)(void *);
    void (*secondFunc)(void *);
    pthread_t* thread;
    int flag;

} AO;

AO* newAO(Queue *queue, void (*firstFunc)(void *), void (*secondFunc)(void *));
void* threadAO(void* activeObject);
void destroyAO(AO* activeObject);
void removeSpecialChars(void* p);
void toUpper(void* p);


// Create a new Active Object:
AO* newAO(Queue *queue, void (*firstFunc)(void *), void (*secondFunc)(void *)){


   AO* activeObject = (AO*)malloc(sizeof(AO));
   activeObject->flag = 0;
   activeObject->queue = queue;
   activeObject->firstFunc = firstFunc;
   activeObject->secondFunc = secondFunc;
   activeObject->thread = (pthread_t*)malloc(sizeof(pthread_t));
   pthread_create((activeObject->thread), NULL, threadAO, (void*)activeObject);
   return activeObject;
}

// Main loop:
void* threadAO(void* activeObject){


    AO* active = (AO*)activeObject;

    active->flag = 1;

    while(active->flag == 1){

        Node* temp = (Node*)deQ(active->queue);
        active->firstFunc(temp);
        active->secondFunc(temp);
    }
}

// Delete an Active Object:
void destroyAO(AO* activeObject) {


    activeObject->flag = 0;
    destroyQ(activeObject->queue);
    free(activeObject);
}


void removeSpecialChars(void* p){
    Node *temp = (Node*) p;

    int len = strlen(temp->proccess);
    
    int i,j;
    for (i=j=0; temp->proccess[i] != '\0'; i++) {
        if (isalnum(temp->proccess[i])) {
            temp->proccess[j++] = temp->proccess[i];
        }
    }
    temp->proccess[j] = '\0';
}

void toUpper(void* p){
    Node *temp = (Node*) p;
    int i;
        for (i = 0; temp->proccess[i] != '\0'; i++) {
            if (!isalpha(temp->proccess[i])) {
                temp->proccess[i] = ' ';
            } else if (islower(temp->proccess[i])) {
                temp->proccess[i] = toupper(temp->proccess[i]);
            }
        }
}

void isPalindrom(void* p){
    Node *temp = (Node*) p;
    int length = strlen(temp->proccess);
    for (int i = 0; i < length/2; i++) {
        if (temp->proccess[i] != temp->proccess[length-i-1]) {
            temp->isPal = 0;
            return;
        }
    }
    temp->isPal = 1;
}




