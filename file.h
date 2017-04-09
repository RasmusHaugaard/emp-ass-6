/*
 * file.h
 *
 *  Created on: 26/12/2011
 *      Author: Morten
 */

#ifndef FILE_H_
#define FILE_H_


typedef int FILE;

#define COM1  0      // UART
#define COM2  1      // LCD
#define COM3  2      // Keyboard

BOOLEAN put_file( FILE, INT8U );
//void putc1( FILE, char );


#endif /* FILE_H_ */
