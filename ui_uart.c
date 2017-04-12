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

typedef struct {
    INT8U condition;
    INT8U state;
    SEM sem;
    INT16U timer;
} TASK_STATUS;

TASK_STATUS task_status[MAX_TASKS];

INT8U i;
INT8U InBuf[128];

enum DEBUG_STATES{
    WAITING_FOR_INSTRUCTION,
    TRANSMITTING_PROCESS_STATUS,
};
enum DEBUG_STATES debug_state = WAITING_FOR_INSTRUCTION;

extern FILE F_UART;

void ui_uart_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
    switch(debug_state){
        case WAITING_FOR_INSTRUCTION: {
            INT8U ch;
            if(file_read(F_UART, &ch)){
                if(i < 128)
                    InBuf[i++] = ch;
                gfprintf(F_UART, (INT8U*)"%c", 0, ch);
                if(ch == '\r'){
                    if(InBuf[0] == 'p' && InBuf[1] == 's'){
                        //TODO: save process status in mem to sync
                        set_state(TRANSMITTING_PROCESS_STATUS);
                    }
                }
            }
            break;
        }
        case TRANSMITTING_PROCESS_STATUS: {
            //TODO: write as much of the state as there can be in the file
            set_state(WAITING_FOR_INSTRUCTION);
            break;
        }
    }
}
