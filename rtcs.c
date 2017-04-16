#include <stdint.h>
#include <math.h>

#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "glob_def.h"
#include "rtcs.h"
#include "systick.h"
#include "string.h"

typedef struct {
  INT8U condition;
  INT8U name[MAX_TASK_NAME_LENGTH];
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

extern volatile INT16U ticks;

INT16U ms, us;

INT16U now_millis(){
    return ms;
}

INT16U now_micros(){
    return us;
}

INT16U millis(INT16U _millis){
    return ceil(_millis * ((float)SYS_FREQ / 1000.0));
}

INT16U micros(INT16U _micros){
    return ceil(_micros * ((float)SYS_FREQ / 1000000.0));
}

TASK current_task;

tcb pot[MAX_TASKS]; // Pool of tasks
scb pos[MAX_SEMAPHORES]; // Pool of semaphores
qcb poq[MAX_QUEUES]; // Pool of queues

INT8U get_task_condition(TASK id){
    return pot[id].condition;
}

const INT8U* get_task_name(TASK id){
    return pot[id].name;
}

INT8U get_task_state(TASK id){
    return pot[id].state;
}

SEM get_task_sem(TASK id){
    return pot[id].sem;
}

INT16U get_task_timer(TASK id){
    return pot[id].timer;
}

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
  wait(millis(500));
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

TASK create_task(void (*tf)(INT8U, INT8U, INT8U, INT8U), INT8U* name){
  static TASK next_id = 0;
  TASK id = next_id++;

  if(id != ERROR_TASK){
	pot[id].condition = TASK_READY;
	strncpy(pot[id].name, name, MAX_TASK_NAME_LENGTH);
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
  for(INT8U i = 0; i < MAX_TASKS; i++){
	pot[i].condition = NO_TASK;
	strcpy(pot[i].name, (INT8U*)"");
  }
  create_task(i_am_alive, (INT8U*)"IM ALIVE");
}

void schedule(){
  init_systick();
  while(TRUE){
    INT16U dticks = ticks;
    ticks -= dticks;

    static INT16U dt_ms_rest, dt_us_rest;
    dt_ms_rest += dticks;
    dt_us_rest += dticks;
    INT16U dms = ((INT32U)dt_ms_rest * 1000) / SYS_FREQ;
    INT16U dus = ((INT32U)dt_us_rest * 1000000) / SYS_FREQ;

    dt_ms_rest -= ((INT32U)dms * SYS_FREQ) / 1000;
    dt_us_rest -= ((INT32U)dus * SYS_FREQ) / 1000000;
    ms += dms;
    us += dus;

    current_task = 0;
    while(pot[current_task].condition != NO_TASK){
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
          if (pot[current_task].timer <= dticks){
              pot[current_task].timer = 0;
              pot[current_task].event = EVENT_TIMEOUT;
              pot[current_task].condition = TASK_READY;
          }else{
              pot[current_task].timer -= dticks;
          }
        }
      }
      if(pot[current_task].condition == TASK_READY)
        pot[current_task].tf(current_task, pot[current_task].state, pot[current_task].event, 0);
   	  current_task++;
	}
  }
}
