#include <stdarg.h>
#include "emp_type.h"
#include "glob_def.h"
#include "rtcs.h"
#include "file.h"

#define NEGATIVE 0
#define POSITIVE 1
#define LEFT    0
#define RIGHT   1

char* strcpy(char* dest, const char *src){
    while (*src != 0)
        *dest++ = *src++;
    return dest;
}

char* strncpy(char* dest, const char *src, INT8U len){
    while (--len > 0)
        *dest++ = *src++;
    return dest;
}

void putStr(FILE fp, const char* str){
  while(*str)
    file_write(fp, *str++);
}

void putChars(FILE fp, const char* str, int len){
  while(len--)
    file_write(fp, *str++);
}

void putDec(FILE fp, long val, int sign, int size, char filler){
  long weight = 1;
  long digit;
  int  i;

  if(sign == NEGATIVE){
    size--;
    if(filler == '0'){
      file_write( fp, '-' );
      sign = POSITIVE;
    }
  }
  if(size == 0){
    while( weight < val ){
      weight *= 10;
      size++;
    }
    if(size == 0)
      size = 1;
    else
      weight /= 10;
  }else{
    i = size - 1;
    while(i-- > 0)
      weight *= 10;
  }

  while(size > 0){
    digit = val / weight;
    if(digit == 0)
      file_write(fp, filler);
    else{
      if(sign == NEGATIVE){
        file_write(fp, '-');
        sign = POSITIVE;
      }
      file_write(fp, digit + '0');
      filler = '0';
    }
    val %= weight;
    weight /= 10;
    size--;
  }
}

void putHex(FILE fp, long val, int size){
  unsigned long weight = 1;
  long digit;
  int  i;

  if(size == 0)
    size = sizeof(long) * 2;

  i = size - 1;
  while(i-- > 0)
    weight *= 16;

  while(size > 0){
    digit = val / weight;
    if(digit < 10)
      file_write(fp, digit + '0');
    else
      file_write(fp, digit + '7');
    val %= weight;
    weight /= 16;
    size--;
  }
}

void gfprintf(FILE fp, const char* str, ...){
  unsigned long val;
  int i, done, size, len, sign, adjust;
  char *subStr, preChar;
  va_list vaArgP;

  va_start(vaArgP, str);

  while(*str){
    // Find the first non-% character, or the end of the string.
    i = 0;
    while(str[i] != '%' && str[i] != '\0')
      i++;

    putChars(fp, str, i);

    // Skip the portion of the string that was written.
    str += i;

    // See if the next character is a %.
    if(*str == '%'){
      size = 0;
      done = FALSE;
      preChar = ' ';
      i = 0;

      while(!done){
        str++;
        switch(*str){
          case '%':
            file_write(fp, '%');
            done = TRUE;
            break;
          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9':
            if(*str == '0' && i == 0)
              preChar = '0';
            size *= 10;
            size += *str - '0';
            i++;
            break;
          case '-':
            if(i == 0)
              adjust = LEFT;
            i++;
            break;
          case 'c':
            val = va_arg(vaArgP, unsigned long);
            file_write(fp, (char)val);
            done = TRUE;
            break;
          case 'd':
            val  = va_arg(vaArgP, unsigned long);
            if((long)val < 0){
              val = -(long)val;
              sign = NEGATIVE;
            }else{
              sign = POSITIVE;
            }
            putDec(fp, (long)val, sign, size, preChar);
            done = TRUE;
            break;
          case 'u':
            val = va_arg(vaArgP, unsigned long);
            putDec(fp, val, POSITIVE, size, preChar);
            done = TRUE;
            break;
          case 'x':
          case 'X':
          case 'p':
            val = va_arg(vaArgP, unsigned long);
            putHex( fp, val, size );
            done = TRUE;
            break;
          case 's':
            subStr = va_arg(vaArgP, char *);
            len = 0;
            while(subStr[len])
              len++;
            if(adjust == RIGHT)
              while(size-- > len)
                file_write(fp, ' ');
            putStr(fp, subStr);
            if(adjust == RIGHT)
              while(size-- > len)
                file_write( fp, ' ' );
            done = TRUE;
            break;
          default:
            putStr(fp, "???");
            done = TRUE;
        }
      }
      str++;
    }
  }
  // End the varargs processing.
  va_end(vaArgP);
}
