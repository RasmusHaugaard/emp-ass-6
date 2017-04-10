#include "emp_type.h"
#include "tmodel.h"
#include "lcd.h"

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
  switch(my_state){
    case 0:
      set_state(1);
      wait(100);
      break;
    case 1:
      sec++;
      if(sec >= 60){
        min++;
        if(min >= 60){
       	  hour++;
          if(hour >= 24)
            hour = 0;
          min = 0;
        }
        sec = 0;
      }
      wait(200); //todo: how does this work correctly?
      signal(SEM_RTC_UPDATED);
      break;
  }
}
//TODO: detach the visual representation from the rtc
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
