#include <stdint.h>
#include "clk.h"
#include "emp_type.h"
#include "tm4c123gh6pm.h"

void set_80MHz(){
    INT32U temp = SYSCTL_RCC_R;
    temp |= SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_USESYSDIV;
    temp &= ~(SYSCTL_RCC_PWRDN | SYSCTL_RCC_BYPASS | SYSCTL_RCC_MOSCDIS);
    SYSCTL_RCC_R = temp;

    temp = SYSCTL_RCC2_R;
    temp |= SYSCTL_RCC2_USERCC2 | SYSCTL_RCC2_DIV400;
    temp &= ~(SYSCTL_RCC2_BYPASS2 | SYSCTL_RCC2_PWRDN2 | SYSCTL_RCC_MOSCDIS | SYSCTL_RCC2_SYSDIV2_M | SYSCTL_RCC2_SYSDIV2LSB);
    temp |= 0x2 << SYSCTL_RCC2_SYSDIV2_S;
    SYSCTL_RCC2_R = temp;
}
