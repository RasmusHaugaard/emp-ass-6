#include "cmd_handler.h"

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

extern FILE F_UART;

#define BUF_SIZE 128
#define MAX_CMDS 8

typedef struct{
    const char* cmd;
    SEM trigger;
} cmd;

cmd poc[MAX_CMDS];
INT8U next_id = 0;

void create_cmd_handler(const char* cmd, SEM trigger)
{
    INT8U id = next_id++;
    if (id < MAX_CMDS){
        poc[id].cmd = cmd;
        poc[id].trigger = trigger;
    }
}

void cmd_handler_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
    static INT8U i = 0;
    static INT8U InBuf[BUF_SIZE];

    INT8U ch;
    while(file_read(F_UART, &ch))
    {
        gfprintf(F_UART, (INT8U*) "%c", 0, ch);
        if (ch == '\r')
        {
            InBuf[i] = '\0';
            for (INT8U i = 0; i < MAX_CMDS; i++){
                if (i < next_id){
                    if (strcmp(InBuf, (INT8U*)poc[i].cmd) == 0){
                        signal(poc[i].trigger);
                    }
                }
            }
            i = 0;
        }
        else
        {
            if (i < BUF_SIZE - 1)
                InBuf[i++] = ch;
        }
    }
}
