#ifndef _RTCS_H
#define _RTCS_H

typedef unsigned char TASK;
typedef unsigned char SEM;
typedef unsigned char QUEUE;

#define MAX_TASKS 16
#define MAX_QUEUES 16
#define MAX_SEMAPHORES 64 // The first 2xMAX_QUEUES are reserved for the queues.

#define NO_TASK 0
#define TASK_READY 1
#define TASK_WAIT_FOR_TIMEOUT 2
#define TASK_WAIT_FOR_SEMAPHORE 4

#define QUEUE_SIZE 128

#define EVENT_NONE 0
#define EVENT_TIMEOUT 1
#define EVENT_SIGNAL 2

#define WAIT_FOREVER 0

#define ERROR_TASK 0xFF // Task error

void set_state(INT8U);
void wait(INT16U);
BOOLEAN wait_sem(INT8U, INT16U);
void signal(INT8U);

QUEUE create_queue();
BOOLEAN put_queue(QUEUE, INT8U, INT16U);
BOOLEAN get_queue(QUEUE, INT8U*, INT16U);

TASK create_task(void (*tf)(INT8U, INT8U, INT8U, INT8U), char* name);
SEM create_sem();

void init_rtcs();
void schedule();

#endif
