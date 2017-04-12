#ifndef _STRING_H
#define _STRING_H

#include "file.h"

typedef struct{
    BOOLEAN finished;
    INT8U index;
} GPRINTF_RESULT;

GPRINTF_RESULT gfprintf(FILE, const INT8U*, INT8U, ...);
INT8U* strcpy(INT8U* dest, const INT8U *src);
INT8U* strncpy(INT8U* dest, const INT8U *src, INT8U len);

#endif

/*
enum TASK_STATES{
    IDLE,
    IMPORTANT_PRINTING,
    LESS_IMPORTANT_PRINTING
}

void process_status_logger(TASK my_task, INT8U my_state, INT8U my_event, INT8U data){
    switch(my_state){
        case IDLE:
            if (i got an event){
                set_state(IMPORTANT_PRINTING);
                wait_sem(UART_TX, WAIT_FOREVER);
            }
            break;
        case IMPORTANT_PRINTING:{
            GPRINTF_RESULT r = gprintf(f, i, "HEJ MED DIG %c!", ch);
            if (r.finished){
                i = 0;
                signal(UART_TX);
                set_state(NEXT_STATE);
            }else{
                i = r.index;
            }
            break;
        }
        case LESS_IMPORTANT_PRINTING:
            gprintf(f, 0, "HEJ MED DIG %c!", ch);
            signal(UART_TX);
            set_state(NEXT_STATE);
            break;
    }
}*/
