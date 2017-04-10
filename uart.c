#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "rtcs.h"

extern QUEUE Q_UART_TX, Q_UART_RX;

BOOLEAN uart_put_q(INT8U ch){
  return put_queue(Q_UART_TX, ch, WAIT_FOREVER);
}

BOOLEAN uart_get_q(INT8U *pch){
  return get_queue(Q_UART_RX, pch, WAIT_FOREVER);
}

BOOLEAN uart0_rx_rdy(){
  return !(UART0_FR_R & UART_FR_RXFE);
}

BOOLEAN uart0_tx_rdy(){
  return !(UART0_FR_R & UART_FR_TXFF);
}

void uart0_putc(INT8U ch){
  UART0_DR_R = ch;
}

extern void uart_rx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
  while(uart0_rx_rdy())
  	put_queue(Q_UART_RX, UART0_DR_R, WAIT_FOREVER);
}

extern void uart_tx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
  INT8U ch;
  while (uart0_tx_rdy() && get_queue(Q_UART_TX, &ch, WAIT_FOREVER))
  	UART0_DR_R = ch;
}

INT32U lcrh_databits(INT8U antal_databits){
  if((antal_databits < 5) || (antal_databits > 8))
	antal_databits = 8;
  return ((INT32U)antal_databits - 5 ) << 5; // Control bit 5-6, WLEN
}

INT32U lcrh_stopbits(INT8U antal_stopbits){
  if(antal_stopbits == 2)
    return 1 << 3;
  else
	return 0;
}

INT32U lcrh_parity(INT8U parity){
  INT32U result;
  switch(parity){
    case 'e':
      result = 0b110;
      break;
    case 'o':
      result = 0b10;
      break;
    case '0':
      result = 0b10000110;
      break;
    case '1':
      result = 0b10000010;
      break;
    case 'n':
    default:
      result = 0;
  }
  return result;
}

void uart0_fifos_enable(){
  UART0_LCRH_R  |= UART_LCRH_FEN;
}

void uart0_fifos_disable(){
  UART0_LCRH_R  &= ~UART_LCRH_FEN;
}

extern void uart0_init(INT32U baud_rate, INT8U databits, INT8U stopbits, INT8U parity){
  INT32U BRD;

  #ifndef E_PORTA
  #define E_PORTA
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; // Enable clock for Port A
  #endif

  #ifndef E_UART0
  #define E_UART0
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0; // Enable clock for UART 0
  #endif

  GPIO_PORTA_AFSEL_R |= 0x00000003;
  GPIO_PORTA_DIR_R |= 0x00000002;
  GPIO_PORTA_DEN_R |= 0x00000003;
  GPIO_PORTA_PUR_R |= 0x00000002;

  BRD = 64000000 / baud_rate;
  UART0_IBRD_R = BRD / 64;
  UART0_FBRD_R = BRD & 0x0000003F;

  UART0_LCRH_R = lcrh_databits(databits);
  UART0_LCRH_R += lcrh_stopbits(stopbits);
  UART0_LCRH_R += lcrh_parity(parity);

  uart0_fifos_enable();

  UART0_CTL_R  |= (UART_CTL_UARTEN | UART_CTL_TXE ); // Enable UART
}
