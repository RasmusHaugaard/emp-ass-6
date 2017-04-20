#include <stdint.h>
#include "tm4c123gh6pm.h"

#include "emp_type.h"
#include "glob_def.h"

#include "clk.h"
#include "gpio.h"
#include "systick.h"
#include "rtcs.h"
#include "string.h"
#include "file.h"

#include "rtc.h"
#include "lcd.h"
#include "uart.h"
#include "key.h"
#include "ui_key.h"
#include "ui_uart.h"

FILE F_UART, F_KEYBOARD, F_LCD;
SEM SEM_RTC_UPDATED;
QUEUE Q_UART_TX, Q_UART_RX, Q_LCD, Q_KEY;

int main(void){
  set_80MHz();

  init_gpio();
  uart0_init(115200, 8, 1, 'n');

  F_UART = create_file(uart_get_q, uart_put_q);
  F_LCD = create_file(NULL, wr_ch_LCD);
  F_KEYBOARD = create_file(get_keyboard, NULL);

  init_rtcs();

  SEM_RTC_UPDATED = create_sem();

  Q_UART_TX = create_queue();
  Q_UART_RX = create_queue();
  Q_LCD = create_queue();
  Q_KEY = create_queue();

  create_task(uart_rx_task, "UART RX");
  create_task(uart_tx_task, "UART TX");
  create_task(ui_uart_task, "UART UI");

  create_task(rtc_task, "RTC");
  create_task(display_rtc_task, "RTC DISP");

  create_task(key_task, "KEY");
  create_task(ui_key_task, "KEY UI");

  create_task(lcd_task, "LCD");

  schedule();
}
