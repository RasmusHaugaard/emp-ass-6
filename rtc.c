#include "emp_type.h"
#include "lcd.h"
#include "rtcs.h"

extern INT8U SEM_RTC_UPDATED;

INT8U sec, min, hour;

INT8U get_hour(){
  return hour;
}

INT8U get_min(){
  return min;
}

INT8U get_sec(){
  return sec;
}

void set_hour(INT8U new_hour){
  hour = new_hour;
}

void set_min(INT8U new_min){
  min = new_min;
}

void set_sec(INT8U new_sec){
  sec = new_sec;
}

void rtc_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
  static INT16U last_millis;
  switch(my_state){
    case 0:
      last_millis = now_millis();
      set_state(1);
      break;
    case 1:
      if ((INT16U)(now_millis() - last_millis) >= 1000){
          last_millis += 1000;
          if(++sec >= 60){
            if(++min >= 60){
              if(++hour >= 24)
                hour = 0;
              min = 0;
            }
            sec = 0;
          }
          signal(SEM_RTC_UPDATED);
      }
      break;
  }
}

void display_rtc_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
  move_LCD(4, 0);

  wr_ch_LCD((INT8U)(hour/10+'0'));
  wr_ch_LCD((INT8U)(hour%10+'0'));
  if(sec & 0x01)
    wr_ch_LCD(':');
  else
    wr_ch_LCD(' ');
  wr_ch_LCD((INT8U)(min/10+'0'));
  wr_ch_LCD((INT8U)(min%10+'0'));
  if(sec & 0x01)
    wr_ch_LCD(' ');
  else
    wr_ch_LCD(':');
  wr_ch_LCD((INT8U)(sec/10+'0'));
  wr_ch_LCD((INT8U)(sec%10+'0'));
  wait_sem(SEM_RTC_UPDATED, WAIT_FOREVER);
}
