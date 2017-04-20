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

#include "cmd_handler.h"
#include "ps.h"


FILE F_UART, F_KEYBOARD, F_LCD;
SEM SEM_RTC_UPDATED, SEM_UART_TX, SEM_CMD_PS_TRIGGER;
QUEUE Q_UART_TX, Q_UART_RX, Q_LCD, Q_KEY;

int main(void){
  set_80MHz();

  init_gpio();
  uart0_init(115200, 8, 1, 'n');

  F_UART = create_file(uart_get_q, uart_put_q);
  F_LCD = create_file(NULL, wr_ch_LCD);
  F_KEYBOARD = create_file(get_keyboard, NULL);

  init_rtcs();

  SEM_RTC_UPDATED = create_sem(0);
  SEM_UART_TX = create_sem(1);
  SEM_CMD_PS_TRIGGER = create_sem(0);

  Q_UART_TX = create_queue();
  Q_UART_RX = create_queue();
  Q_LCD = create_queue();
  Q_KEY = create_queue();

  create_cmd_handler("ps", SEM_CMD_PS_TRIGGER);

  create_task(uart_rx_task, "UART RX");
  create_task(uart_tx_task, "UART TX");

  create_task(rtc_task, "RTC");
  create_task(display_rtc_task, "RTC DISP");

  create_task(key_task, "KEY");
  create_task(ui_key_task, "KEY UI");

  create_task(lcd_task, "LCD");

  create_task(cmd_handler_task, "CMD HANDL");
  create_task(cmd_ps_task, "CMD_PS");

  schedule();
}
