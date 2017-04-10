#include "ui_key.h"
#include "file.h"
#include "lcd.h"
#include "string.h"

extern FILE F_KEYBOARD, F_UART, F_LCD;

void ui_key_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
  INT8U ch;
  if(file_read(F_KEYBOARD, &ch)){
    gfprintf(F_UART, "Du tastede: %c\r\n", ch);
    gfprintf(F_LCD, "%c%cDu tastede: %c", ESC, 0xc0, ch);
  }
}
