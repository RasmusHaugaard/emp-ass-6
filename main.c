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

void ui_key_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
  INT8U ch;

  if( get_file( COM3, &ch ))
  {
    gfprintf( COM1, "Du tastede: %c\r\n", ch );
    gfprintf( COM2, "%c%cDu tastede: %c", ESC, 0xc0, ch );
  }

    //put_queue( Q_UART_TX, ch, WAIT_FOREVER );

}

int main(void)
/*****************************************************************************
*   Input    : NONE
*   Output   : Returns 0
*   Function : Init hardware and then loop forever
******************************************************************************/
{
  init_gpio();

  uart0_init( 9600, 8, 1, 'n' );

  init_rtcs();
  init_files();


  open_queue( Q_UART_TX );
  open_queue( Q_UART_RX );
  open_queue( Q_LCD );
  open_queue( Q_KEY );

  start_task( TASK_RTC, rtc_task );
  start_task( TASK_DISPLAY_RTC, display_rtc_task );
  start_task( TASK_LCD, lcd_task );
  start_task( TASK_UART_TX, uart_tx_task );
  start_task( TASK_UART_RX, uart_rx_task );
  start_task( TASK_UI, ui_task );
  start_task( TASK_KEY, key_task );
  start_task( TASK_UI_KEY, ui_key_task );

  schedule();
  return( 0 );
}
