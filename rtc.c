#include "emp_type.h"
#include "lcd.h"
#include "rtcs.h"
#include "interval.h"

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
  static INT8U interval;
  switch(my_state){
    case 0:
      interval = create_interval(millis(1000));
      set_state(1);
      break;
    case 1:
      if (check_interval(interval)){
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
  INT8U i = 4;

  wr_ch_LCD((INT8U)(hour/10+'0'), i++);
  wr_ch_LCD((INT8U)(hour%10+'0'), i++);
  if(sec & 0x01)
    wr_ch_LCD(':', i++);
  else
    wr_ch_LCD(' ', i++);
  wr_ch_LCD((INT8U)(min/10+'0'), i++);
  wr_ch_LCD((INT8U)(min%10+'0'), i++);
  if(sec & 0x01)
    wr_ch_LCD(' ', i++);
  else
    wr_ch_LCD(':', i++);
  wr_ch_LCD((INT8U)(sec/10+'0'), i++);
  wr_ch_LCD((INT8U)(sec%10+'0'), i++);

  wait_sem(SEM_RTC_UPDATED, WAIT_FOREVER);
}
