#include "ui_uart.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"
#include "systick.h"
#include "systick.h"
#include "rtc.h"
#include "file.h"
#include "string.h"
#include "rtcs.h"

typedef struct
{
    INT8U condition;
    INT8U state;
    SEM sem;
    INT16U timer;
} TASK_STATUS;

TASK_STATUS task_status[MAX_TASKS];

INT8U i = 0;
INT8U InBuf[128];

enum DEBUG_STATES
{
    WAITING_FOR_INSTRUCTION, TRANSMITTING_PROCESS_STATUS,
};

extern FILE F_UART;

void ui_uart_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    switch (my_state)
    {
    case WAITING_FOR_INSTRUCTION:
    {
        INT8U ch;
        if (file_read(F_UART, &ch))
        {
            if (i < 128)
                InBuf[i++] = ch;
            gfprintf(F_UART, (INT8U*) "%c", 0, ch);
            if (ch == '\r')
            {
                i = 0;
                if (InBuf[0] == 'p' && InBuf[1] == 's')
                {
                    i = i;
                    for (INT8U i = 0; i < MAX_TASKS; i++)
                    {
                        task_status[i].condition = get_task_condition(i);
                        task_status[i].sem = get_task_sem(i);
                        task_status[i].state = get_task_state(i);
                        task_status[i].timer = get_task_timer(i);
                    }
                    set_state(TRANSMITTING_PROCESS_STATUS);
                }
            }
        }
        break;
    }
    case TRANSMITTING_PROCESS_STATUS:
    {
        static INT8U cur_task = 0;
        static INT8U char_index = 0;

        INT8U* name = get_task_name(cur_task);
        INT8U cond = task_status[cur_task].condition;
        INT8U state = task_status[cur_task].state;
        INT16U time = task_status[cur_task].timer;
        INT8U sem = task_status[cur_task].sem;

        GPRINTF_RESULT r;

        if (cond == 0){
            r = gfprintf(F_UART, (INT8U*)"Task: %02u %-10s, Condition: Dead\n", char_index, cur_task, name);
        }else if(cond == TASK_READY){
            r = gfprintf(F_UART, (INT8U*)"Task: %02u %-10s, Condition: Ready  , State: %03u\n", char_index, cur_task, name, state);
        }else if(cond == (TASK_WAIT_FOR_TIMEOUT & TASK_WAIT_FOR_SEMAPHORE)){
            r = gfprintf(F_UART, (INT8U*)"Task: %02u %-10s, Condition: Waiting, State: %03u, Sem: %03u, Tim: %05u\n", char_index, cur_task, name, state, sem, time);
        }else if(cond == TASK_WAIT_FOR_SEMAPHORE){
            r = gfprintf(F_UART, (INT8U*)"Task: %02u %-10s, Condition: Waiting, State: %03u, Sem: %03u,\n", char_index, cur_task, name, state, sem);
        }else if(cond == TASK_WAIT_FOR_TIMEOUT){
            r = gfprintf(F_UART, (INT8U*)"Task: %02u %-10s, Condition: Waiting, State: %03u,         , Tim: %05u\n", char_index, cur_task, name, state, time);
        }

        if (r.finished)
        {
            cur_task++;
            char_index = 0;
            if (cur_task == MAX_TASKS)
            {
                cur_task = 0;
                set_state(WAITING_FOR_INSTRUCTION);
            }
        }
        else
        {
            char_index = r.index;
        }
        break;
    }
    }
}
