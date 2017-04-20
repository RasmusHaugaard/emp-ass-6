#ifndef _RTCS_H
#define _RTCS_H

#include <stdint.h>
#include "emp_type.h"

typedef unsigned char TASK;
typedef unsigned char SEM;
typedef unsigned char QUEUE;

#define MAX_TASKS 16
#define MAX_QUEUES 16
#define MAX_SEMAPHORES 64 // The first 2xMAX_QUEUES are reserved for the queues.

#define TASK_NAME_LENGTH 10

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

INT16U now_ticks();
INT16U now_millis();

INT16U millis(INT16U);
INT16U micros(INT16U);

void set_state(INT8U);
void wait(INT16U);
BOOLEAN wait_sem(INT8U, INT16U);
void signal(INT8U);

QUEUE create_queue();
BOOLEAN put_queue(QUEUE, INT8U, INT16U);
BOOLEAN get_queue(QUEUE, INT8U*, INT16U);

TASK create_task(void (*tf)(INT8U, INT8U, INT8U, INT8U), char* name);
SEM create_sem(INT8U init_val);

INT8U get_task_condition(TASK id);
const INT8U* get_task_name(TASK id);
INT8U get_task_state(TASK id);
SEM get_task_sem(TASK id);
INT16U get_task_timer(TASK id);

void init_rtcs();
void schedule();

#endif
