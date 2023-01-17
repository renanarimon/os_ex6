
# pragma once

#include "ActiveObject.h"

void enSecond(void* node);
void enThird(void* node);
void enFourth(void* node);

Queue* q1;
Queue* q2;
Queue* q3;
Queue* q4;


// Pipline
typedef struct _Pipeline {

    AO *first;
    AO *second;
    AO *third;
    AO *fourth;

} Pipeline;

// Create a new Pipeline:
Pipeline* createPipeline(Queue* q1, Queue* q2, Queue* q3,Queue* q4, void (*firstFunc)(void *), void (*secondFunc)(void *), void (*thirdFunc)(void *), void (*fourthFunc)(void *)){


    Pipeline* pipe = (Pipeline*)(malloc(sizeof(Pipeline)));

    pipe->first = newAO(q1, firstFunc, enSecond);
    pipe->second = newAO(q2, secondFunc, enThird);
    pipe->third = newAO(q3, thirdFunc, enFourth);
    pipe->fourth = newAO(q4, fourthFunc, deleteNode);

    return pipe;
}

void destroyPipeline(Pipeline* pipe){


    destroyAO(pipe->first);
    destroyAO(pipe->second);
    destroyAO(pipe->third);

    free(pipe);

}

void enSecond(void* node){


    enQ(q2, node);
}

void enThird(void* node){


    enQ(q3, node);
}
void enFourth(void* node){


    enQ(q4, node);
}


