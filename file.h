#ifndef FILE_H_
#define FILE_H_

typedef int FILE;

//TODO: make the instantation more generic / dynamic
#define COM1  0      // UART
#define COM2  1      // LCD
#define COM3  2      // Keyboard

void init_files();
BOOLEAN put_file(FILE, INT8U);
BOOLEAN get_file(FILE, INT8U*);

#endif
