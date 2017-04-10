#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "glob_def.h"
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

FILE UART;
FILE KEYBOARD;
FILE LCD;

void ui_key_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
  INT8U ch;
  if(file_read(KEYBOARD, &ch)){
    gfprintf(UART, "Du tastede: %c\r\n", ch);
    gfprintf(LCD, "%c%cDu tastede: %c", ESC, 0xc0, ch);
  }
}

int main(void){
  init_gpio();
  uart0_init( 9600, 8, 1, 'n' );
  init_files();

  UART = create_file(uart_get_q, uart_put_q);
  LCD = create_file(NULL, wr_ch_LCD);
  KEYBOARD = create_file(get_keyboard, NULL);

  init_rtcs();
  open_queue(Q_UART_TX);
  open_queue(Q_UART_RX);
  open_queue(Q_LCD);
  open_queue(Q_KEY);

  create_task(rtc_task, "RTC");
  create_task(display_rtc_task, "RTC DISP");
  create_task(lcd_task, "LCD");
  create_task(uart_tx_task, "UART TX");
  create_task(uart_rx_task, "UART RX");
  create_task(ui_task, "UI");
  create_task(key_task, "KEY");
  create_task(ui_key_task, "KEY UI");

  schedule();
}
