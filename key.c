/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: key.c
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
* 150321  MoH   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "tmodel.h"

INT8U row( INT8U y )
{
  INT8U result = 0;

  switch( y )
  {
    case 0x01: result = 1; break;
    case 0x02: result = 2; break;
    case 0x04: result = 3; break;
    case 0x08: result = 4; break;
  }
  return( result );
}



INT8U key_catch( x, y )
INT8U x, y;
{
  const INT8U matrix[3][4] = {{'#','9','6','3'},
                              {'0','8','5','2'},
                              {'*','7','4','1'}};

  return( matrix[x-1][y-1] );

}

BOOLEAN get_keyboard( INT8U *pch )
{
  return( get_queue( Q_KEY, pch, WAIT_FOREVER ));
}

extern void key_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
/*****************************************************************************
*   Input    :
*   Output   :
*   Function :
******************************************************************************/
{
  INT8U y;
  INT8U ch;
  static INT8U x = 1;

  switch( my_state )
  {
    case 0:
      GPIO_PORTA_DATA_R |= 0x04;
      GPIO_PORTF_DATA_R |= 0x0E;
      set_state( 1 );

      break;
    case 1:
      y = GPIO_PORTE_DATA_R & 0x0F;
      if( y )
      {
        GPIO_PORTF_DATA_R &= 0xFD;
        ch = key_catch( x, row(y) );
        put_queue( Q_KEY, ch, 1 );
        set_state( 2 );
      }
      else
      {
        x++;
        if( x > 3 )
          x = 1;
        GPIO_PORTA_DATA_R &= 0xE3;
        GPIO_PORTA_DATA_R |= ( 0x01 << (x+1 ));
      }
      break;
    case 2:
      if( !(GPIO_PORTE_DATA_R & 0x0F ))
      {
        x=1;
        GPIO_PORTA_DATA_R |= 0x04;
        set_state( 1 );
      }
      break;
  }
}
