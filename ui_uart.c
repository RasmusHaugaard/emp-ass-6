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

INT8U i;
INT8U InBuf[128];

extern FILE F_UART;

void ui_uart_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
  INT8U ch;
  if(file_read(F_UART, &ch)){
    if(i < 128)
      InBuf[i++] = ch;
    gfprintf(F_UART, "%c", ch);
    if(ch == '\r'){
      if(InBuf[0] == '2'){
        gfprintf(F_UART, "2%02d%02d%02d", get_hour(), get_min(), get_sec());
      }
      if((InBuf[0] == '1') && (i >= 7)){
          set_hour((InBuf[1]-'0')*10+InBuf[2]-'0');
          set_min((InBuf[3]-'0')*10+InBuf[4]-'0');
          set_sec((InBuf[5]-'0')*10+InBuf[6]-'0');
          gfprintf(F_UART, "%02d%02d%02d", get_hour(), get_min(), get_sec());
      }
      gfprintf(F_UART, "\r\n>>");
      i = 0;
    }
  }
}
