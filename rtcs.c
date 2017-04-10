#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "glob_def.h"
#include "rtcs.h"
#include "systick.h"
#include "string.h"

#define TASK_NAME_LENGTH 10

typedef struct {
  INT8U condition;
  char name[TASK_NAME_LENGTH];
  INT8U state;
  INT8U event;
  SEM sem;
  INT16U timer;
  void (*tf)(INT8U, INT8U, INT8U, INT8U);  // Pointer to task function
} tcb; //task control block

typedef struct
{
  INT8U condition;
  INT8U type;
  INT8U count;
} scb; //semaphore control block

typedef struct
{
	INT8U head;
	INT8U tail;
	SEM q_not_full;
	SEM q_not_empty;
	INT8U buf[QUEUE_SIZE];
} qcb; //queue control block

extern volatile INT16S ticks;

TASK current_task;

tcb pot[MAX_TASKS]; // Pool of tasks
scb pos[MAX_SEMAPHORES]; // Pool of semaphores
qcb poq[MAX_QUEUES]; // Pool of queues

void i_am_alive(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
  if(my_state == 0){
    // Turn on the LED.
    GPIO_PORTD_DATA_R |= 0x40;
    set_state(1);
  }else{
    // Turn off the LED.
    GPIO_PORTD_DATA_R &= ~(0x40);
    set_state(0);
  }
  wait(200);
}

void set_state(INT8U new_state){
  pot[current_task].state = new_state;
}

void wait(INT16U timeout){
  pot[current_task].timer = timeout;
  pot[current_task].condition = TASK_WAIT_FOR_TIMEOUT;
}

BOOLEAN wait_sem(SEM sem, INT16U timeout){
  BOOLEAN result = TRUE;
  if(pos[sem].count){
	pos[sem].count--;
    pot[current_task].condition = TASK_READY;
	result = TRUE;
  }else{
    pot[current_task].sem = sem;
    pot[current_task].condition = TASK_WAIT_FOR_SEMAPHORE;
    if(timeout){
	  pot[current_task].timer = timeout;
	  pot[current_task].condition |= TASK_WAIT_FOR_TIMEOUT;
    }
    result = FALSE;
  }
  return result;
}

void signal(SEM sem){
  if(sem < MAX_SEMAPHORES)
	pos[sem].count++;
}

void preset_sem(SEM sem, INT8U signals){
  if(sem < MAX_SEMAPHORES)
    pos[sem].count = signals;
}

QUEUE create_queue(){
    static QUEUE next_id = 0;
    QUEUE id = next_id++;
    if(id < MAX_QUEUES){
      poq[id].head = 0;
      poq[id].tail = 0;
      poq[id].q_not_full = id;
      poq[id].q_not_empty = MAX_QUEUES + id;
      preset_sem(poq[id].q_not_full, QUEUE_SIZE);
    }
    return id;
}

BOOLEAN put_queue(INT8U id, INT8U ch, INT16U timeout){
  BOOLEAN result = FALSE;
  if(id < MAX_QUEUES){
	if(wait_sem(poq[id].q_not_full, timeout)){
      poq[id].buf[poq[id].head++] = ch;
      poq[id].head &= 0x7F;
      signal(poq[id].q_not_empty);
      result = TRUE;
    }
  }
  return result;
}

BOOLEAN get_queue(QUEUE id, INT8U* pch, INT16U timeout){
  BOOLEAN result = FALSE;
  if(id < MAX_QUEUES){
	if(wait_sem(poq[id].q_not_empty, timeout)){
      *pch = poq[id].buf[poq[id].tail++];
      poq[id].tail &= 0x7F;
      signal(poq[id].q_not_full);
      result = TRUE;
    }
  }
  return result;
}

TASK create_task(void (*tf)(INT8U, INT8U, INT8U, INT8U), char* name){
  static TASK next_id = 0;
  TASK id = next_id++;

  if(id != ERROR_TASK){
	pot[id].condition = TASK_READY;
	strncpy(pot[id].name, name, TASK_NAME_LENGTH);
	pot[id].state = 0;
	pot[id].timer = 0;
	pot[id].tf = tf;
  }
  return id;
}

SEM create_sem(){
    static SEM next_id = 2 * MAX_QUEUES;
    SEM id = next_id++;
    return id;
}

void init_rtcs(){
  init_systick();
  for(INT8U i = 0; i < MAX_TASKS; i++)
	pot[i].condition = NO_TASK;
  create_task(i_am_alive, "IM ALIVE");
}

void schedule(){
  while(1){
    while(!ticks); //TODO: maybe update as fast as possible and allow interval timers
    ticks--;
    current_task = 0;
    do{
      if(pot[current_task].condition & TASK_WAIT_FOR_SEMAPHORE){
        if(pos[pot[current_task].sem].count){
          if(pot[current_task].sem >= 2 * MAX_QUEUES)
              pos[pot[current_task].sem].count--;
          pot[current_task].event = EVENT_SIGNAL;
          pot[current_task].condition = TASK_READY;
        }
      }
      if(pot[current_task].condition & TASK_WAIT_FOR_TIMEOUT){
        if(pot[current_task].timer){
          if(! --pot[current_task].timer){
            pot[current_task].event = EVENT_TIMEOUT;
            pot[current_task].condition = TASK_READY;
          }
        }
      }
      if(pot[current_task].condition == TASK_READY)
        pot[current_task].tf(current_task, pot[current_task].state, pot[current_task].event, 0);
   	  current_task++;
	} while (pot[current_task].condition != NO_TASK);
  }
}
