#ifndef _LCD_H
  #define _LCD_H

#define LF 0x0A
#define FF 0x0C
#define CR 0x0D

#define ESC 0x1B

void wr_ch_LCD(INT8U ch, INT8U index);
void wr_str_LCD(const INT8U* str, INT8U index);

void lcd_task(INT8U, INT8U, INT8U, INT8U);

#endif

