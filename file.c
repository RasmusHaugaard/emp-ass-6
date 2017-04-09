/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: file.c
*
* PROJECT....: EMP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 150322  MoH   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
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

/*****************************    Defines    *******************************/
#define MAX_FILES  8

typedef struct
{
  BOOLEAN (*put)(INT8U);   // Pointer to device put function
  BOOLEAN (*get)(INT8U*);  // Pointer to device get function
} fcb;  //file control block

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
fcb pof[MAX_FILES];	            // Pool of foles

/*****************************   Functions   *******************************/

BOOLEAN put_file( FILE file, INT8U ch )
{
  if( pof[file].put )
    pof[file].put( ch );
}

BOOLEAN get_file( FILE file, INT8U *pch )
{
  if( pof[file].get )
    pof[file].get( pch );
}


extern INT8U init_files()
/*****************************************************************************
*   Function : See module specification (.h-file).
*****************************************************************************/
{
  INT8U i;

  for( i = 0; i < MAX_FILES; i++ )
  {
	  pof[i].put = NULL;
	  pof[i].get = NULL;
  }
  pof[COM1].put = uart_put_q;
  pof[COM1].get = uart_get_q;
  pof[COM2].put = wr_ch_LCD;
  pof[COM3].get = get_keyboard;

  return( 1 );
}

/****************************** End Of Module *******************************/












