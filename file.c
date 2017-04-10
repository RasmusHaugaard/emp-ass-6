#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "glob_def.h"
#include "rtcs.h"
#include "file.h"
#include "key.h"
#include "uart.h"
#include "lcd.h"

#define MAX_FILES 8

typedef struct {
  BOOLEAN (*put)(INT8U); // Pointer to device put function
  BOOLEAN (*get)(INT8U*); // Pointer to device get function
} fcb; //file control block

fcb pof[MAX_FILES]; // Pool of files
INT8U next_file_id = 0;

BOOLEAN file_write(FILE file, INT8U ch){
  if(pof[file].put)
    return pof[file].put(ch);
  return FALSE;
}

BOOLEAN file_read(FILE file, INT8U *pch){
  if(pof[file].get)
    return pof[file].get(pch);
  return FALSE;
}

void init_files(){
  for(INT8U i = 0; i < MAX_FILES; i++){
	  pof[i].put = NULL;
	  pof[i].get = NULL;
  }
}

FILE create_file(BOOLEAN (*get)(INT8U*), BOOLEAN (*put)(INT8U)){
    FILE id = next_file_id++;
    pof[id].get = get;
    pof[id].put = put;
    return id;
}
