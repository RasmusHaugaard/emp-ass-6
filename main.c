#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"
#include "systick.h"
#include "tmodel.h"
#include "systick.h"
#include "rtc.h"
#include "lcd.h"
#include "uart.h"
#include "ui.h"
#include "key.h"
#include "string.h"
#include "file.h"

void ui_key_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
  INT8U ch;
  if(get_file(COM3, &ch)){
    gfprintf(COM1, "Du tastede: %c\r\n", ch);
    gfprintf(COM2, "%c%cDu tastede: %c", ESC, 0xc0, ch);
  }
}

int main(void){
  init_gpio();

  uart0_init( 9600, 8, 1, 'n' );

  init_rtcs();
  init_files();
  //TODO: open files

  open_queue(Q_UART_TX);
  open_queue(Q_UART_RX);
  open_queue(Q_LCD);
  open_queue(Q_KEY);

  TASK_RTC = create_task(rtc_task, "RTC");
  TASK_DISPLAY_RTC = create_task(display_rtc_task, "RTC DISP");
  TASK_LCD = create_task(lcd_task, "LCD");
  TASK_UART_TX = create_task(uart_tx_task, "UART TX");
  TASK_UART_RX = create_task(uart_rx_task, "UART RX");
  TASK_UI = create_task(ui_task, "UI");
  TASK_KEY = create_task(key_task, "KEY");
  TASK_UI_KEY = create_task(ui_key_task, "KEY UI");

  schedule();
}
