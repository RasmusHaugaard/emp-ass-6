#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"

void init_gpio(void){
  // Enable the GPIO port that is used for the on-board LED.
  SYSCTL_RCGC2_R  =  SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOC | SYSCTL_RCGC2_GPIOD | SYSCTL_RCGC2_GPIOE | SYSCTL_RCGC2_GPIOF;
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0;

  __asm__("nop");
  __asm__("nop");

  // Set the direction .
  GPIO_PORTA_DIR_R = 0x1C;
  GPIO_PORTC_DIR_R = 0xF0;
  GPIO_PORTD_DIR_R = 0x4C;
  GPIO_PORTE_DIR_R = 0x00;
  GPIO_PORTF_DIR_R = 0x0E;

  // Enable the GPIO pins for digital function.
  GPIO_PORTA_DEN_R = 0x1C;
  GPIO_PORTC_DEN_R = 0xF0;
  GPIO_PORTD_DEN_R = 0x4C;
  GPIO_PORTE_DEN_R = 0x0F;
  GPIO_PORTF_DEN_R = 0x1F;

  // Enable internal pull-up (PF0 and PF4).
  GPIO_PORTF_PUR_R = 0x11;
}
