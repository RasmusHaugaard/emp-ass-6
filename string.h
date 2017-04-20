#ifndef _STRING_H
#define _STRING_H

#include "file.h"

typedef struct{
    BOOLEAN finished;
    INT8U index;
} GPRINTF_RESULT;

GPRINTF_RESULT gfprintf(FILE, const INT8U*, INT8U, ...);
INT8U* strcpy(INT8U* dest, const INT8U *src);
INT8U* strncpy(INT8U* dest, const INT8U *src, INT8U len);
INT8S strcmp(const INT8U* a, const INT8U* b);

#endif
