/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: key.h
*
* PROJECT....: EMP
*
* DESCRIPTION: Test.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 150321  MoH   Module created.
*
*****************************************************************************/

#ifndef _KEY_H
  #define _KEY_H

BOOLEAN get_keyboard( INT8U* );
extern void key_task(INT8U, INT8U, INT8U, INT8U );


#endif
