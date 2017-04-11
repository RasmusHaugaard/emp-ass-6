#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "lcd.h"
#include "glob_def.h"
#include "rtcs.h"

#define QUEUE_LEN 128

extern QUEUE Q_LCD;

enum LCD_states
{
  LCD_POWER_UP,
  LCD_INIT,
  LCD_READY,
  LCD_ESC_RECEIVED,
};

const INT8U LCD_init_sequense[]= 
{
  0x30,		// Reset
  0x30,		// Reset
  0x30,		// Reset
  0x20,		// Set 4bit interface
  0x28,		// 2 lines Display
  0x0C,		// Display ON, Cursor OFF, Blink OFF
  0x06,		// Cursor Increment
  0x01,		// Clear Display
  0x02,     	// Home
  0xFF		// stop
}; 

enum LCD_states LCD_state = LCD_POWER_UP;
INT8U LCD_init;

BOOLEAN wr_ch_LCD(INT8U Ch){
  return put_queue(Q_LCD, Ch, WAIT_FOREVER);
}

void wr_str_LCD(INT8U *str){
  while(*str){
    wr_ch_LCD(*str);
	str++;
  }
}

void move_LCD(INT8U x, INT8U y){
  INT8U Pos;

  Pos = y*0x40 + x;
  Pos |= 0x80;
  wr_ch_LCD(ESC);
  wr_ch_LCD(Pos);
}

void wr_ctrl_LCD_low(INT8U Ch){
  INT8U temp;
  volatile int i;
  //TODO: should not busy wait
  temp = GPIO_PORTC_DATA_R & 0x0F;
  temp  = temp | ((Ch & 0x0F) << 4);
  GPIO_PORTC_DATA_R  = temp;
  for( i=0; i<5000; i )
	  i++;
  GPIO_PORTD_DATA_R &= 0xFB; // Select Control mode, write
  for( i=0; i<5000; i )
	  i++;
  GPIO_PORTD_DATA_R |= 0x08; // Set E High
  for( i=0; i<5000; i )
	  i++;
  GPIO_PORTD_DATA_R &= 0xF7;	// Set E Low
  for( i=0; i<5000; i )
	  i++;
}

void wr_ctrl_LCD_high(INT8U Ch){
  wr_ctrl_LCD_low((Ch & 0xF0) >> 4);
}

//TODO: according to specification there are time requirements between below instructions
void out_LCD_low(INT8U Ch){
  INT8U temp;
  temp = GPIO_PORTC_DATA_R & 0x0F;
  GPIO_PORTC_DATA_R  = temp | ((Ch & 0x0F) << 4);
  GPIO_PORTD_DATA_R |= 0x04; // Select data mode
  GPIO_PORTD_DATA_R |= 0x08;	// Set E High
  GPIO_PORTD_DATA_R &= 0xF7;	// Set E Low
  for(INT8U i = 0; i < 100; i++);
}

void out_LCD_high( INT8U Ch ){
  out_LCD_low((Ch & 0xF0) >> 4);
}

void wr_ctrl_LCD(INT8U Ch){
  static INT8U Mode4bit = FALSE;
  INT16U i;
  wr_ctrl_LCD_high(Ch);
  if(Mode4bit){
	for(i = 0; i < 5000; i++); //TODO: remove busy wait
	wr_ctrl_LCD_low(Ch);
  }else{
	if((Ch & 0x30) == 0x20)
	  Mode4bit = TRUE;
  }
}

void clr_LCD(){
  wr_ctrl_LCD(0x01);
}

void home_LCD(){
  wr_ctrl_LCD(0x02);
}

void Set_cursor(INT8U Ch){
  wr_ctrl_LCD(Ch);
}

void out_LCD(INT8U Ch){
  INT16U i;
  out_LCD_high(Ch);
  for(i=0; i<5000; i++); //TODO: should not busy wait
  out_LCD_low( Ch );
}

//TODO: continuously update LCD from an image buffer
void lcd_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
  INT8U ch;
  switch(my_state){
    case LCD_POWER_UP:
      LCD_init = 0;
      set_state(LCD_INIT);
      wait(millis(100));
      break;
    case LCD_INIT:
      if(LCD_init_sequense[LCD_init] != 0xFF){
        wr_ctrl_LCD(LCD_init_sequense[LCD_init++]);
      }else{
		set_state(LCD_READY);
	  }
	  wait(millis(40));
      break;
    case LCD_READY:
      if(get_queue(Q_LCD, &ch, WAIT_FOREVER)){
        switch( ch ){
	      case 0xFF:
	        clr_LCD();
	        break;
	      case ESC:
		    set_state(LCD_ESC_RECEIVED);
		    break;
	      default:
		    out_LCD(ch);
		}
	  }
	  break;
	case LCD_ESC_RECEIVED:
	  if(get_queue(Q_LCD, &ch, WAIT_FOREVER)){
		if(ch & 0x80){
		    Set_cursor(ch);
		}else{
		  switch(ch){
		    case '@':
		    	home_LCD();
			  break;
		  }
        }
	    set_state(LCD_READY);
	    wait(millis(50));
      }
	  break;
  }
}
