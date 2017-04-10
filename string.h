#ifndef _STRING_H
#define _STRING_H

#include "file.h"

void gfprintf(FILE, const char*, ...);
char* strcpy(char* dest, const char *src);
char* strncpy(char* dest, const char *src, INT8U len);

#endif
