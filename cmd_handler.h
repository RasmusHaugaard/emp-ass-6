#ifndef _CMD_HANDLER_H
#define _CMD_HANDLER_H
#include "emp_type.h"
#include "rtcs.h"

void cmd_handler_task(INT8U, INT8U, INT8U, INT8U);
void create_cmd_handler(const char* cmd, SEM trigger);

#endif
