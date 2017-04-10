#ifndef _TMODEL_H_
#define _TMODEL_H_

#include "rtcs.h"

INT8U TASK_IM_ALIVE,
      TASK_UART_RX,
      TASK_KEY,
      TASK_UI,
      TASK_UI_KEY,
      TASK_UART_TX,
      TASK_RTC,
      TASK_DISPLAY_RTC,
      TASK_LCD;

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
