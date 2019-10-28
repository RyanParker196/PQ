//Partner: Ryan Parker

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "pqueue.jhibbele.h"

//======================================================================================================================

typedef enum EventTypeEnum {
    PROCESS_SUBMITTED,
    PROCESS_STARTS,
    PROCESS_ENDS,
    PROCESS_TIMESLICE_EXPIRES,
} EventType;

typedef struct {
    int pid;
    int burstTime;
    int waitTime;
    int numPreemptions;
    int lastTime;
} Process;

typedef struct {
    EventType eventType;
    Process *process;
} Event;

int arr[1000];

double mean;

//======================================================================================================================

Process *createProcesses() {
    Process *processes = (Process *) malloc(5 * sizeof(Process));
    int burstTimes[5] = {6, 7, 2, 5, 2};
    int lastTimes[5] = {0, 3, 4, 6, 6};
    for (int i = 0; i < 5; ++i) {
        processes[i].pid = i + 1;
        processes[i].lastTime = lastTimes[i];
        processes[i].burstTime = burstTimes[i];
    }
    return processes;
}

//======================================================================================================================

void enqueueProcesses(PQueueNode **eventPQueue, Process *processes, int numProcesses) {
    for (int i = 0; i < numProcesses; ++i) {
        Event *event;
        event = (Event *) malloc(sizeof(Event));
        event->eventType = PROCESS_SUBMITTED;
        event->process = &processes[i];
        enqueue(eventPQueue, event->process->lastTime, event);
    }
}

//======================================================================================================================

void printEvent(Event *event) {
    Process *process;
    process = event->process;
    if (event->eventType == PROCESS_SUBMITTED)
        printf("T = %d PROCESS_SUBMITTED pid = %d\n",
               process->lastTime, process->pid);
    else if (event->eventType == PROCESS_STARTS)
        printf("T = %d PROCESS_STARTS pid = %d\n",
               process->lastTime, process->pid);
    else if (event->eventType == PROCESS_ENDS)
        printf("T = %d PROCESS_ENDS pid = %d\n",
               process->lastTime + process->burstTime, process->pid);
}

//======================================================================================================================

int genExpRand(double mean) {
    double r, t;
    int rtnval ;
    r = drand48();
    t = -log(1-r) * mean;
    rtnval = (int) floor(t);
    if (rtnval == 0) {
        rtnval = 1;
    }
    return (rtnval);
}

//======================================================================================================================

Process *createRandomProcesses(int numProcesses , double meanBurstTime) {
    Process *processes = (Process *) malloc(numProcesses * sizeof(Process));
    for (int i = 0; i < numProcesses; ++i) {
        processes[i].pid = i + 1; // start the process IDs at 1 instead of zero
        processes[i].waitTime = 0;
        processes[i].lastTime = 0;
        processes[i].numPreemptions = 0;
        processes[i].burstTime = (int) genExpRand(25);
    }
    return processes;
}

//======================================================================================================================

void enqueueRandomProcesses(int numProcesses, PQueueNode **eventQueue, Process *processes, double meanIAT) {
    int t = 0;
    for (int i = 0; i < numProcesses; ++i ) {
        Event *event;
        event = (Event *) malloc(sizeof(Event));
        memset(event, 0, sizeof(Event));
        event->eventType = PROCESS_SUBMITTED;
        event->process = &processes[i];
        enqueue(eventQueue, t, event) ;
        t = t + (int) genExpRand(meanIAT);
    }
}

//======================================================================================================================

void runSimulation(int schedulerType, int quantum, PQueueNode *eventPQueue){
    Process *process;
    int totalWaitTime = 0;
    int busy = 0;
    PQueueNode *processQueue = NULL;
    Event *newEvent = NULL;
    int currentTime = getMinPriority(eventPQueue);
    Event *event = dequeue(&eventPQueue);

    while (event != NULL) {
        process = event->process;
        if (event->eventType == PROCESS_SUBMITTED) {
            process->waitTime = currentTime;
            if (busy == 0) {
                printf("T = %d PROCESS_SUBMITTED pid = %d\n", currentTime, process->pid);
                newEvent = (Event *) malloc(sizeof(Event));
                newEvent->eventType = PROCESS_STARTS;
                newEvent->process = process;
                enqueue(&eventPQueue, currentTime, newEvent);
                busy = 1;
            } else {
                printf("T = %d PROCESS_SUBMITTED pid = %d\n", currentTime, process->pid);

                if (schedulerType == 2) {
                    enqueue(&processQueue, process->burstTime, process);
                } else if (schedulerType == 1) {
                    enqueue(&processQueue, 0, process);
                } else if (schedulerType == 3) {
                    enqueue(&processQueue, currentTime, process);
                }
            }
        } else if (event->eventType == PROCESS_STARTS) {
            if (schedulerType == 3 && process->burstTime > quantum) {
                newEvent = (Event *) malloc(sizeof(Event));
                newEvent->eventType = PROCESS_TIMESLICE_EXPIRES;
                process->waitTime += quantum;
                newEvent->process = process;
                enqueue(&eventPQueue, currentTime + quantum, newEvent);
            } else {
                printf("T = %d PROCESS_STARTS pid = %d\n", currentTime, process->pid);
                process->waitTime = currentTime - process->waitTime;
                totalWaitTime += process->waitTime;
                newEvent = (Event *) malloc(sizeof(Event));
                newEvent->eventType = PROCESS_ENDS;
                newEvent->process = process;
                enqueue(&eventPQueue, currentTime + process->burstTime, newEvent);
            }
        } else if (event->eventType == PROCESS_TIMESLICE_EXPIRES) {
            process->burstTime = process->burstTime - quantum;
            printf("T = %d PROCESS TIMESLICE EXPIRES pid = %d\n", currentTime, process->pid);
            enqueue(&processQueue, currentTime, process);
            if (queueLength(processQueue) > 0) {
                process = dequeue(&processQueue);
                newEvent = (Event *) malloc(sizeof(Event));
                newEvent->eventType = PROCESS_STARTS;
                newEvent->process = process;
                enqueue(&eventPQueue, currentTime, newEvent);
            } else {
                busy = 0;
            }
        } else if (event->eventType == PROCESS_ENDS) {
            printf("T = %d PROCESS_ENDS pid = %d wait time = %d\n", currentTime, process->pid, process->waitTime);
            arr[process->pid - 1] = process->waitTime;
            if (queueLength(processQueue) > 0) {
                process = dequeue(&processQueue);
                newEvent = (Event *) malloc(sizeof(Event));
                newEvent->eventType = PROCESS_STARTS;
                newEvent->process = process;
                enqueue(&eventPQueue, currentTime, newEvent);
                busy = 0;
            } else {
                busy = 0;
            }
        }
        currentTime = getMinPriority(eventPQueue);
        event = dequeue(&eventPQueue);
    }
    printf("\n");
    printf("Mean wait time = %.2f\n", (double) totalWaitTime / 5);
    mean = totalWaitTime / 1000;
}

//======================================================================================================================

int main() {
//    unsigned short seed = 1;
//    seed48(&seed);
//
//    PQueueNode *eventPQ = NULL;
//    Process* processes = createRandomProcesses(1000, 25);
//    enqueueRandomProcesses(1000, &eventPQ, processes, 25);
//
//    runSimulation(2, 4, eventPQ);
//
//    double total = 0;
//    for (int p = 0; p < 1000; ++p) {
//        double difference = arr[p] - mean;
//        total = total + pow(difference,2);
//    }
//
//    double std = sqrt(total / 1000);
//
//    printf("%.2f\n",std);

    Process* processes = createProcesses();
    PQueueNode *eventPQ = NULL;
    enqueueProcesses(&eventPQ, processes, 5);

    //change schedulerType to 2 for SFJ, 3 for RR, and 1 for FCFS
    runSimulation(3, 4, eventPQ);
}