#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "lcd.h"
#include "glob_def.h"
#include "rtcs.h"

enum lcd_life_states{
  LCD_POWER_UP,
  LCD_INIT,
  LCD_ALIVE,
};
enum lcd_life_states lcd_life_state = LCD_POWER_UP;

enum lcd_wr_states{
    WR_DATA_E_HIGH,
    WR_E_LOW,
};
enum lcd_wr_states wr_state = WR_DATA_E_HIGH;

enum reg_modes{
    REG_NONE,
    REG_CTRL,
    REG_DATA
};
enum reg_modes reg_mode = REG_NONE;

enum write_part{
    WHOLE,
    HIGH
};

typedef struct {
    INT8U data;
    write_part wp;
    FP32 wait_ms;
} lcd_instr;

lcd_instr init_seq[] = {
    {0x30, HIGH, 4.1}, // Reset
    {0x30, HIGH, 0.1}, // Reset
    {0x30, HIGH, 0.1}, // Reset
    {0x20, HIGH, 0.037}, // Set 4bit interface
    {0x28, WHOLE, 0.037}, // 2 lines Display
    {0x0C, WHOLE, 0.037}, // Display ON, Cursor OFF, Blink OFF
    {0x06, WHOLE, 0.037}, // Cursor Increment
    {0x01, WHOLE, 2}, // Clear Display
    {0x02, WHOLE, 1.52}, // Home
    {0xFF, WHOLE, 0} // init 4bit done
};
INT8U init_4bit_state;

INT8U image_buffer[32];

void init_image_buffer(){
    for (INT8U i = 0; i < 32; i++)
        image_buffer[i] = ' ';
}

void wr_str_LCD(const INT8U *str, INT8U index){
  while(*str && index < 32)
    image_buffer[index++] = *str++;
}

void wr_strn_LCD(const INT8U *str, INT8U index, INT8U len){
  while(*str && index < 32 && len--)
    image_buffer[index++] = *str++;
}

void wr_ctrl_LCD_low(INT8U Ch){
  INT8U temp = GPIO_PORTC_DATA_R & ~0xF0;
  GPIO_PORTC_DATA_R = temp | ((Ch & 0x0F) << 4);

  GPIO_PORTD_DATA_R &= ~0x04; // select control register mode
  //wait 60 ns
  GPIO_PORTD_DATA_R |= 0x08; // Set E High
  //wait 450 ns
  GPIO_PORTD_DATA_R &= 0xF7;	// Set E Low
  //wait 20 ns
}

void wr_ctrl_LCD_high(INT8U Ch){
  wr_ctrl_LCD_low((Ch & 0xF0) >> 4);
}

//TODO: according to specification there are time requirements between below instructions
void out_LCD_low(INT8U Ch){
    INT32U temp = GPIO_PORTC_DATA_R & ~0xF0;
    GPIO_PORTC_DATA_R = temp | ((Ch & 0x0F) << 4); //set new data

    GPIO_PORTD_DATA_R |= 0x04; // Select data register mode
    //wait 60 ns
    GPIO_PORTD_DATA_R |= 0x08; // Set E High
    //wait 450 ns
    GPIO_PORTD_DATA_R &= ~0x08; // Set E Low
    //wait 20 ns
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





void wr_ctrl_LCD(INT8U data, enum lcd_life_states next_state){
  INT16U i;
  wr_ctrl_LCD_high(data);
  if(Mode4bit){
    for(i = 0; i < 5000; i++); //TODO: remove busy wait
    wr_ctrl_LCD_low(data);
  }else{
    if((Ch & 0x30) == 0x20)
      Mode4bit = TRUE;
  }
}

void lcd_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
    INT8U wr_data;
    INT32U temp;
    switch(my_state){
        case LCD_POWER_UP:
            init_state = 0;
            set_state(LCD_INIT);
            wait(millis(40));
            break;
        case LCD_INIT: {
            static BOOLEAN wait = FALSE;
            if (!wait){
                if(init_seq[init_state].data != 0xFF){
                    wr_ctrl_LCD(init_seq[init_state].data);
                }else{
                    set_state(LCD_ALIVE);
                }
            }else{
                wait(millis(init_seq[init_state].wait_ms));
                init_state++;
            }
        }

            break;
        case LCD_ALIVE:
            switch(alive_state){
                case AL_HOME_CURSOR:
                    switch (homing_state){
                        case sdf:
                            break;
                    }
                    break;
                case AL_WRITE_FROM_BUFFER:

                    break;
            }
            switch(wr_state){
                case WR_DATA_E_HIGH:
                    temp = GPIO_PORTC_DATA_R & ~0xF0;
                    GPIO_PORTC_DATA_R = temp | ((ch & 0xF) << 4);

                    GPIO_PORTD_DATA_R |= 0x08;
                    wr_state = WR_E_LOW;
                    wait(micros(.5));
                    break;
                case WR_E_LOW:
                    GPIO_PORTD_DATA_R &= ~0x08;
                    wr_state = WR_DATA_E_HIGH;
                    wait(micros(.5));
                    break;
            }
        break;
    }
}
