#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "glob_def.h"
#include "rtcs.h"
#include "file.h"
#include "tmodel.h"
#include "key.h"
#include "uart.h"
#include "lcd.h"

#define MAX_FILES 8

typedef struct {
  BOOLEAN (*put)(INT8U); // Pointer to device put function
  BOOLEAN (*get)(INT8U*); // Pointer to device get function
} fcb; //file control block

fcb pof[MAX_FILES]; // Pool of files

BOOLEAN put_file(FILE file, INT8U ch){
  if(pof[file].put)
    return pof[file].put(ch);
  return FALSE;
}

BOOLEAN get_file(FILE file, INT8U *pch){
  if(pof[file].get)
    return pof[file].get(pch);
  return FALSE;
}

extern void init_files(){
  for(INT8U i = 0; i < MAX_FILES; i++){
	  pof[i].put = NULL;
	  pof[i].get = NULL;
  }
  //TODO: make the initialization part generic / dynamic
  pof[COM1].put = uart_put_q;
  pof[COM1].get = uart_get_q;
  pof[COM2].put = wr_ch_LCD;
  pof[COM3].get = get_keyboard;
}
