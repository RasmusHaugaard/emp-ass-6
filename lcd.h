#ifndef _LCD_H
  #define _LCD_H

#define LF 0x0A
#define FF 0x0C
#define CR 0x0D

#define ESC 0x1B

void wr_str_LCD(INT8U*);
void move_LCD(INT8U, INT8U);
BOOLEAN wr_ch_LCD(INT8U Ch);

void lcd_task(INT8U, INT8U, INT8U, INT8U);

#endif

