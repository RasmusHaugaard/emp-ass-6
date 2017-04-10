#ifndef FILE_H_
#define FILE_H_

#include "emp_type.h"

typedef INT8U FILE;

void init_files();
BOOLEAN file_write(FILE, INT8U);
BOOLEAN file_read(FILE, INT8U*);
FILE create_file(BOOLEAN (*read)(INT8U*), BOOLEAN (*write)(INT8U));

#endif
