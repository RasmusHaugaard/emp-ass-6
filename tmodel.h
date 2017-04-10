#ifndef _TMODEL_H_
#define _TMODEL_H_

#include "rtcs.h"

//TODO: init these dynamically
#define TASK_UART_RX USER_TASK
#define TASK_KEY USER_TASK + 1
#define TASK_UI USER_TASK + 2
#define TASK_UI_KEY USER_TASK + 3
#define TASK_UART_TX USER_TASK + 4
#define TASK_RTC USER_TASK + 5
#define TASK_DISPLAY_RTC USER_TASK + 6
#define TASK_LCD USER_TASK + 7

#define ISR_TIMER 21

#define SEM_LCD USER_SEM
#define SEM_RTC_UPDATED USER_SEM + 1

#define SSM_RTC_SEC 31
#define SSM_RTC_MIN 32
#define SSM_RTC_HOUR 33

#define Q_UART_TX USER_QUEUE
#define Q_UART_RX USER_QUEUE + 1
#define Q_LCD USER_QUEUE + 2
#define Q_KEY USER_QUEUE + 3

#endif
