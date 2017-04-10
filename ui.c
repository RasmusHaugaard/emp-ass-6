#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"
#include "systick.h"
#include "tmodel.h"
#include "systick.h"
#include "ui.h"
#include "rtc.h"
#include "file.h"
#include "string.h"

INT8U i;
INT8U InBuf[128];

void ui_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
  INT8U ch;
  if(get_file(COM1, &ch)){
    if(i < 128)
      InBuf[i++] = ch;
    gfprintf(COM1, "%c", ch);
    if(ch == '\r'){
      if(InBuf[0] == '2'){
        gfprintf(COM1, "2%02d%02d%02d", get_hour(), get_min(), get_sec());
      }
      if((InBuf[0] == '1') && (i >= 7)){
          set_hour((InBuf[1]-'0')*10+InBuf[2]-'0');
          set_min((InBuf[3]-'0')*10+InBuf[4]-'0');
          set_sec((InBuf[5]-'0')*10+InBuf[6]-'0');
          gfprintf(COM1, "%02d%02d%02d", get_hour(), get_min(), get_sec());
      }
      gfprintf(COM1, "\r\n>>");
      i = 0;
    }
  }
}
