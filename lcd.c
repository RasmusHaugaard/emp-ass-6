#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "lcd.h"
#include "glob_def.h"
#include "rtcs.h"
#include <math.h>

#define MAX_CHARS 32

INT8U image_buffer[MAX_CHARS];

enum task_states{
    LCD_POWER_UP,
    LCD_INIT,
    LCD_ALIVE,
    LCD_WR,
};

enum wr_states{
    WR_REG_MODE,
    WR_DATA_E_HIGH,
    WR_E_LOW,
};
enum wr_states wr_state;

enum reg_modes{
    REG_NONE,
    REG_CTRL,
    REG_DATA
};
enum reg_modes reg_mode;

enum bit_modes{
    BIT4,
    BIT8
};
enum bit_modes bit_mode;

typedef struct {
    INT8U data;
    enum bit_modes bit_mode;
    FP32 wait_ms;
} lcd_instr;

lcd_instr init_seq[] = {
    {0x30, BIT8, 4.1}, // Reset
    {0x30, BIT8, 0.1}, // Reset
    {0x30, BIT8, 0.1}, // Reset
    {0x20, BIT8, 0.037}, // Set 4bit interface
    {0x28, BIT4, 0.037}, // 2 lines Display
    {0x0C, BIT4, 0.037}, // Display ON, Cursor OFF, Blink OFF
    {0x06, BIT4, 0.037}, // Cursor Increment
    {0x01, BIT4, 2}, // Clear Display
    {0x02, BIT4, 1.52}, // Home
    {0xFF, BIT4, 0.1} // init done
};
INT8U init_state = 0;

void init_image_buffer(){
    for (INT8U i = 0; i < 32; i++)
        image_buffer[i] = ' ';
}

void wr_ch_LCD(INT8U ch, INT8U index){
    if (index < MAX_CHARS)
        image_buffer[index] = ch;
}

void wr_str_LCD(const INT8U *str, INT8U index){
  while(*str && index < MAX_CHARS)
    image_buffer[index++] = *str++;
}

void wr_strn_LCD(const INT8U *str, INT8U index, INT8U len){
  while(*str && index < MAX_CHARS && len--)
    image_buffer[index++] = *str++;
}

INT8U wr_data;
INT16U delay;
enum task_states next_state;
void wr_LCD(INT8U data, enum reg_modes rm, enum bit_modes bm, enum task_states next_ts, INT16U del)
{
    wr_data = data;
    bit_mode = bm;
    next_state = next_ts;
    delay = del;
    set_state(LCD_WR);
    if (reg_mode != rm){
        reg_mode = rm;
        wr_state = WR_REG_MODE;
    }else{
        wr_state = WR_DATA_E_HIGH;
    }
}

void set_pos(INT8U pos, enum task_states next_ts)
{
    wr_LCD(0x80 | pos, REG_CTRL, BIT4, next_ts, millis(0.037));
}

void wr_ch(INT8U ch, enum task_states next_ts)
{
    wr_LCD(ch, REG_DATA, BIT4, next_ts, 0);
}

void lcd_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data){
    switch(my_state){
        case LCD_POWER_UP:
            init_image_buffer();
            init_state = 0;
            set_state(LCD_INIT);
            wait(millis(40));
            break;
        case LCD_INIT:
            if(init_seq[init_state].data != 0xFF){
                wr_LCD(
                    init_seq[init_state].data, REG_CTRL,
                    init_seq[init_state].bit_mode, LCD_INIT,
                    init_seq[init_state].wait_ms
                );
                init_state++;
            }else{
                set_state(LCD_ALIVE);
            }
            break;
        case LCD_ALIVE:{
            static INT8S x = -1;
            static INT8U y = 0;
            if (x == -1){
                x = 0;
                set_pos(y * 0x40, LCD_ALIVE);
            }else{
                wr_ch(image_buffer[y * (MAX_CHARS/2) + x++], LCD_ALIVE);
                if (x == MAX_CHARS / 2){
                    x = -1;
                    y = (y + 1) % 2;
                }
            }
            break; //TODO: maybe not break here and implement idle case in LCD_WR
        }
        case LCD_WR:
            switch(wr_state){
                case WR_REG_MODE:
                    if (reg_mode == REG_CTRL)
                        GPIO_PORTD_DATA_R &= ~0x04;
                    else //REG_DATA
                        GPIO_PORTD_DATA_R |= 0x04;

                    wr_state = WR_DATA_E_HIGH;
                    wait(micros(.06)); //Address setup time (t_AS = 60 ns)
                    break;
                case WR_DATA_E_HIGH: {
                    INT32U temp = GPIO_PORTC_DATA_R & ~0xF0;
                    GPIO_PORTC_DATA_R = temp | (wr_data & 0xF0); //Set data
                    GPIO_PORTD_DATA_R |= 0x08; //Set E high
                    wr_state = WR_E_LOW;
                    wait(micros(.5)); //tcycle / 2 = 500ns > Enable Pulse Width (PW_EH 450 ns) > Data set-up time (t_DSW = 196 ns)
                    break;
                }
                case WR_E_LOW:
                    GPIO_PORTD_DATA_R &= ~0x08; //Set E low
                    wr_state = WR_DATA_E_HIGH;
                    if (bit_mode == BIT4){
                        bit_mode = BIT8;
                        wr_data = wr_data << 4;
                        wait(micros(.5)); //tcycle / 2 = 500ns > Address Hold Time (t_AH = 20 ns) > Data Hold Time (t_H = 10 ns)
                    }else{
                        if (micros(.5) < delay)
                            wait(delay);
                        else
                            wait(micros(.5));
                        set_state(next_state);
                    }
                    break;
            }
        break;
    }
}
