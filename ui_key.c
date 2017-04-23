#include "ui_key.h"
#include "file.h"
#include "lcd.h"
#include "string.h"
#include "rtcs.h"

extern FILE F_KEYBOARD, F_UART, F_LCD;
extern SEM SEM_UART_TX;

void ui_key_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
  static INT8U ch;
  switch(my_state)
      {
      case 0:
          if(file_read(F_KEYBOARD, &ch))
          {
              wr_str_LCD("Du tastede: ", 16);
              wr_ch_LCD(ch, 28);
              set_state(1);
              wait_sem(SEM_UART_TX, WAIT_FOREVER);
          }
          break;
      case 1:{
          static INT8U char_i = 0;
          GPRINTF_RESULT r = gfprintf(F_UART, (INT8U*)"Du tastede: %c\r\n", char_i, ch);
          char_i = r.index;
          if (r.finished)
          {
              char_i = 0;
              signal(SEM_UART_TX);
              set_state(0);
          }
          break;
      }
  }
}
