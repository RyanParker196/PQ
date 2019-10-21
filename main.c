#include <stdio.h>
#include <stdlib.h>
#include "pqueue.jhibler.h"

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

Process *createProcesses(){
    Process *processes = (Process *) malloc(5 * sizeof(Process));

    int burstTimes[5] = {6, 7, 2, 5, 2};
    int lastTimes[5] = {0, 3, 4, 6, 6};
    for (int i = 0; i < 5; ++i) {
        processes[i].pid = i +1;
        processes[i].lastTime = lastTimes[i];
        processes[i].burstTime = burstTimes[i];
    }
    return processes;
}

void enqueueProcesses(PQueueNode **eventPQueue, Process *processes, int numProcesses){
    for (int i = 0; i < numProcesses; ++i) {
        Event *event;
        event = (Event *) malloc(sizeof(Event));
        event->eventType = PROCESS_SUBMITTED;
        event->process = &processes[i];

        enqueue(eventPQueue, event->process->lastTime, event);
    }
}

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

void runSimulation(int schedulerType, int quantum, PQueueNode *eventPQueue){
    Process *process;
    int waitTime = 0;
    int totalWaitTime = 0;
    int busy = 0;
    PQueueNode *processQueue = NULL;
    Event *newEvent = NULL;
    int currentTime = getMinPriority(eventPQueue);
    Event *event = dequeue(&eventPQueue);
    while (event != NULL){

        process = event->process;
        if(event->eventType == PROCESS_SUBMITTED){
            process->waitTime = currentTime;
            if (busy == 0){
                //printEvent(event);
                printf("T = %d PROCESS_SUBMITTED pid = %d\n", currentTime, process->pid);
                newEvent = (Event *) malloc(sizeof(Event));
                newEvent->eventType = PROCESS_STARTS;
                newEvent->process = process;
                enqueue(&eventPQueue, currentTime, newEvent);
                busy = 1;
            } else{
                //printEvent(event);
                printf("T = %d PROCESS_SUBMITTED pid = %d\n", currentTime, process->pid);

                if (schedulerType == 2)
                {
                    enqueue(&processQueue,process->burstTime,process);
                }else if (schedulerType == 1){
                    enqueue(&processQueue,0,process);
                }

            }
        } else if(event->eventType == PROCESS_STARTS){
            //printEvent(event);
            printf("T = %d PROCESS_STARTS pid = %d\n", currentTime, process->pid);
            process->waitTime = currentTime - process->waitTime;
            totalWaitTime += process->waitTime;

            newEvent = (Event *) malloc(sizeof(Event));
            newEvent->eventType = PROCESS_ENDS;
            newEvent->process = process;
            enqueue(&eventPQueue, currentTime + process->burstTime, newEvent);
        } else if(event->eventType == PROCESS_ENDS){
            //printEvent(event);
            printf("T = %d PROCESS_ENDS pid = %d wait time = %d\n", currentTime, process->pid,process->waitTime);


            if (queueLength(processQueue) > 0){
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
    printf("%d processes; Mean wait time = %.2f\n", 5, (double) totalWaitTime/5);
}
int main(){
    Process* processes = createProcesses();
    PQueueNode *eventPQ = NULL;
    enqueueProcesses(&eventPQ,processes,5);

    //change schedulerType to 2 for SFJ
    runSimulation(1,0,eventPQ);
}
