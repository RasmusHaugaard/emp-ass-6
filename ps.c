#include "emp_type.h"
#include "file.h"
#include "rtcs.h"
#include "ps.h"
#include "string.h"

typedef struct
{
    INT8U condition;
    INT8U state;
    SEM sem;
    INT16U timer;
} TASK_STATUS;

TASK_STATUS task_status[MAX_TASKS];

extern FILE F_UART;
extern SEM SEM_CMD_PS_TRIGGER;
extern SEM SEM_UART_TX;

void cmd_ps_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    switch (my_state)
    {
        case 0:
            wait_sem(SEM_CMD_PS_TRIGGER, WAIT_FOREVER);
            set_state(1);
            break;
        case 1:
        {
            for (INT8U i = 0; i < MAX_TASKS; i++)
            {
                task_status[i].condition = get_task_condition(i);
                task_status[i].sem = get_task_sem(i);
                task_status[i].state = get_task_state(i);
                task_status[i].timer = get_task_timer(i);
            }
            wait_sem(SEM_UART_TX, WAIT_FOREVER);
            set_state(2);
            break;
        }
        case 2:
        {
            static INT8U cur_task = 0;
            static INT8U char_index = 0;

            const INT8U* name = get_task_name(cur_task);
            INT8U cond = task_status[cur_task].condition;
            INT8U state = task_status[cur_task].state;
            INT16U time = task_status[cur_task].timer;
            INT8U sem = task_status[cur_task].sem;

            GPRINTF_RESULT r;

            if (cond == NO_TASK)
            {
                r = gfprintf(F_UART, (INT8U*) "Task: %02u %-10s, Condition: Dead\n",
                             char_index, cur_task, name);
            }
            else if (cond == TASK_READY)
            {
                r =
                        gfprintf(
                                F_UART,
                                (INT8U*) "Task: %02u %-10s, Condition: Ready  , State: %03u\n",
                                char_index, cur_task, name, state);
            }
            else if (cond == (TASK_WAIT_FOR_TIMEOUT & TASK_WAIT_FOR_SEMAPHORE))
            {
                r =
                        gfprintf(
                                F_UART,
                                (INT8U*) "Task: %02u %-10s, Condition: Waiting, State: %03u, Sem: %03u, Tim: %05u\n",
                                char_index, cur_task, name, state, sem, time);
            }
            else if (cond == TASK_WAIT_FOR_SEMAPHORE)
            {
                r =
                        gfprintf(
                                F_UART,
                                (INT8U*) "Task: %02u %-10s, Condition: Waiting, State: %03u, Sem: %03u,\n",
                                char_index, cur_task, name, state, sem);
            }
            else if (cond == TASK_WAIT_FOR_TIMEOUT)
            {
                r =
                        gfprintf(
                                F_UART,
                                (INT8U*) "Task: %02u %-10s, Condition: Waiting, State: %03u,         , Tim: %05u\n",
                                char_index, cur_task, name, state, time);
            }
            char_index = r.index;
            if (r.finished)
            {
                cur_task++;
                char_index = 0;
                if (cur_task == MAX_TASKS)
                {
                    cur_task = 0;
                    set_state(0);
                    signal(SEM_UART_TX);
                }
            }
            break;
        }
    }
}
