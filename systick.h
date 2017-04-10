#ifndef _EMP_H
#define _EMP_H

#define CLK 16000000
#define SYSTICK_RELOAD_VALUE 1600
#define SYS_FREQ (CLK / SYSTICK_RELOAD_VALUE)

extern volatile INT16U ticks;

void init_systick();

#endif
