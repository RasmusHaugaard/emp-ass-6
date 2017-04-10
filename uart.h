#ifndef _UART_H
#define _UART_H

BOOLEAN uart_put_q(INT8U);
BOOLEAN uart_get_q(INT8U*);

void uart_tx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data);
void uart_rx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data);

extern void uart0_init( INT32U, INT8U, INT8U, INT8U );

#endif
