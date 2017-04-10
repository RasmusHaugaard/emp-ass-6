#ifndef _RTCS_H
#define _RTCS_H

typedef unsigned char HANDLE; // HANDLE: Task diagram element ID
typedef unsigned char SEM;

#define MAX_TASKS 16
#define MAX_QUEUES 16
#define MAX_SEMAPHORES 64 // The first 2xMAX_QUEUES are reserved for the queues.

#define USER_QUEUE 0
#define USER_SEM 2 * MAX_QUEUES

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

extern void set_state(INT8U);
extern void wait(INT16U);
extern BOOLEAN wait_sem(INT8U, INT16U);
extern void signal(INT8U);
INT8S open_queue(INT8U);
BOOLEAN put_queue(INT8U, INT8U, INT16U);
BOOLEAN get_queue(INT8U, INT8U*, INT16U);

extern HANDLE create_task(void (*tf)(INT8U, INT8U, INT8U, INT8U), char* name);
extern void init_rtcs();
void schedule();

#endif
