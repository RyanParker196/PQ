#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pqueue.jhibler.h"

//----------------------------------------------------------------------

int enqueue(PQueueNode **pqueue, int priority, void *data) {
    PQueueNode *newNode, *currNode, *prevNode;
    newNode = (PQueueNode *) malloc(sizeof(PQueueNode));
    newNode->next = NULL;
    newNode->data = data;
    newNode->priority = priority;

    if (*pqueue == NULL) {
        *pqueue = newNode;
    } else {
        prevNode = NULL;
        currNode = *pqueue;
        while (currNode != NULL && priority >= currNode->priority) {
            prevNode = currNode;
            currNode = currNode->next;
        }

        if (currNode == NULL) {
            // insert at end
            prevNode->next = newNode;
        } else {
            if (prevNode != NULL) {
                // insert in middle
                prevNode->next = newNode;
                newNode->next = currNode;
            } else {
                // insert at front
                newNode->next = currNode;
                *pqueue = newNode;
            }
        }
    }
    return(0);
}

//----------------------------------------------------------------------

void printQueue(PQueueNode *pqueue, void (printFunction)(void *) ) {
    PQueueNode *currNode;
    currNode = pqueue;
    while (currNode != NULL) {
        printf("priority = %d", currNode->priority);
        printf(" data = ");
        (*printFunction)(currNode->data);
        currNode = currNode->next;
    }
}

//----------------------------------------------------------------------

void printStudentRecord(void *data) {
    StudentRecord *rec;
    rec = (StudentRecord *) data;
    printf("%s %d\n", rec->name, rec->id);
}

//----------------------------------------------------------------------

int getMinPriority(PQueueNode *pqueue) {
    if (pqueue == NULL)
        return(-1);
    else
        return(pqueue->priority);
}

//----------------------------------------------------------------------

void *peek(PQueueNode *pqueue) {
    if (pqueue == NULL)
        return(NULL);
    else
        return(pqueue->data);
}

//----------------------------------------------------------------------

void *dequeue(PQueueNode **pqueue) {
    if (*pqueue == NULL)
        return(NULL);
    else {
        PQueueNode *nextNode;
        void *data;
        data = (*pqueue)->data;
        nextNode = (*pqueue)->next;
        free(*pqueue);
        *pqueue = nextNode;
        return(data);
    }
}

//----------------------------------------------------------------------


int queueLength(PQueueNode *pqueue) {
    int count = 0;
    PQueueNode *currNode;
    currNode = pqueue;
    while (currNode != NULL) {
        ++count;
        currNode = currNode->next;
    }
    return(count);
}
