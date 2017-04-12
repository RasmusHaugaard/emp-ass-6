#include <stdarg.h>
#include "emp_type.h"
#include "glob_def.h"
#include "rtcs.h"
#include "file.h"
#include "string.h"

#define NEGATIVE 0
#define POSITIVE 1
#define LEFT    0
#define RIGHT   1

INT8U* strcpy(INT8U* dest, const INT8U* src){
    while (*src != 0)
        *dest++ = *src++;
    return dest;
}

INT8U* strncpy(INT8U* dest, const INT8U* src, INT8U len){
    while (--len > 0)
        *dest++ = *src++;
    return dest;
}

BOOLEAN wr_c(FILE fp, INT8U ch, INT8U start_i, INT8U* global_i){
    if (start_i > *global_i)
        return TRUE;
    if (file_write(fp, ch)){
        ++*global_i;
        return TRUE;
    }
    return FALSE;
}

BOOLEAN putStr(FILE fp, const INT8U* str, INT8U start_i, INT8U* global_i){
  while(*str)
    if(!wr_c(fp, *str++, start_i, global_i))
        return FALSE;
  return TRUE;
}

BOOLEAN putDec(FILE fp, long val, int sign, int size, char filler, INT8U start_i, INT8U* global_i){
  INT32S weight = 1;
  INT32S digit;
  INT16S i;

  if(sign == NEGATIVE){
    size--;
    if(filler == '0'){
      if(!wr_c(fp, '-', start_i, global_i))
        return FALSE;
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
    if(digit == 0){
      if(!wr_c(fp, filler, start_i, global_i))
        return FALSE;
    }else{
      if(sign == NEGATIVE){
        if(!wr_c(fp, '-', start_i, global_i))
          return FALSE;
        sign = POSITIVE;
      }
      if(!wr_c(fp, digit + '0', start_i, global_i))
        return FALSE;
      filler = '0';
    }
    val %= weight;
    weight /= 10;
    size--;
  }
  return TRUE;
}

BOOLEAN putHex(FILE fp, INT32U val, INT8U size, INT8U start_i, INT8U* global_i){
  INT32U weight = 1;
  INT32S digit;
  INT16S i;

  if(size == 0)
    size = sizeof(INT32S) * 2;

  i = size - 1;
  while(i-- > 0)
    weight *= 16;

  while(size > 0){
    digit = val / weight;
    if(digit < 10)
      digit += '0';
    else
      digit += '7';

    if (!wr_c(fp, digit, start_i, global_i))
        return FALSE;

    val %= weight;
    weight /= 16;
    size--;
  }
  return TRUE;
}

GPRINTF_RESULT gfprintf(FILE fp, const INT8U* str, INT8U start_i, ...){
  INT32U val;
  INT8U global_i = 0;
  INT8U i, size, len;
  BOOLEAN done, sign, adjust;
  INT8U *subStr, preChar;
  BOOLEAN wr_err = FALSE;

  va_list vaArgP;
  va_start(vaArgP, start_i);

  while(*str && !wr_err){
    i = 0;
    while(str[i] != '%' && str[i] != '\0' && !wr_err){
        wr_err = !wr_c(fp, str[i], start_i, &global_i);
        i++;
    }
    // Skip the portion of the string that was written.
    str += i;

    if(*str == '%'){
      size = 0;
      done = FALSE;
      preChar = ' ';
      adjust = RIGHT;
      i = 0;

      while(!done && !wr_err){
        str++;
        switch(*str){
          case '%':
            wr_err = !wr_c(fp, '%', start_i, &global_i);
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
            val = va_arg(vaArgP, INT32U);
            wr_err = !wr_c(fp, (INT8U)val, start_i, &global_i);
            done = TRUE;
            break;

          case 'd':
            val = va_arg(vaArgP, INT32U);
            if((INT32S)val < 0){
              val = -(INT32S)val;
              sign = NEGATIVE;
            }else{
              sign = POSITIVE;
            }
            wr_err = !putDec(fp, (INT32S)val, sign, size, preChar, start_i, &global_i);
            done = TRUE;
            break;

          case 'u':
            val = va_arg(vaArgP, INT32U);
            wr_err = !putDec(fp, val, POSITIVE, size, preChar, start_i, &global_i);
            done = TRUE;
            break;

          case 'x': case 'X': case 'p':
            val = va_arg(vaArgP, INT32U);
            wr_err = !putHex(fp, val, size, start_i, &global_i);
            done = TRUE;
            break;

          case 's':
            subStr = va_arg(vaArgP, INT8U*);
            len = 0;
            while(subStr[len])
              len++;
            if(adjust == RIGHT)
              while(size-- > len && !wr_err)
                  wr_err = !wr_c(fp, ' ', start_i, &global_i);
            if (wr_err)
                break;
            if (!putStr(fp, subStr, start_i, &global_i))
                break;
            if(adjust == LEFT)
              while(size-- > len && !wr_err)
                wr_err = !wr_c(fp, ' ', start_i, &global_i);
            done = TRUE;
            break;

          default:
            wr_err = !putStr(fp, (INT8U*)"???", start_i, &global_i);
            done = TRUE;
        }
      }
      str++;
    }
  }
  // End the varargs processing.
  va_end(vaArgP);
  GPRINTF_RESULT r = {!wr_err, global_i};
  return r;
}
