////////////////////////////////////////////////////////////////////////////////
// toStr.cpp
////////////////////////////////////////////////////////////////////////////////

#include "toStr.h"

namespace RV
{
  char* toStr(uint32_t val, char *buff, uint8_t size, char leadingChar, bool hex)
  {
    char *b = buff + size-1 ;

    if (val)
    {
      if (hex)
      {
        static char digit[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' } ;
        while (val && (b >= buff))
        {
          *(b--) = digit[val & 0x0f] ;
          val >>= 4 ;
        }
      }
      else
      {
        while (val && (b >= buff))
        {
          *(b--) = '0' + (val % 10) ;
          val /= 10 ;
        }
      }
    }
    else
    {
      *(b--) = '0' ;
    }

    if (val) // overflow
    {
      for (uint8_t i = 0 ; i < size ; ++i)
        buff[i] = '*' ;
      return nullptr ;
    }
    
    if (!leadingChar)
      return ++b ;

    for (char *z = buff ; z <= b ; ++z)
      *z = leadingChar ;

    return buff ;
  }

  char* toStr(int32_t val, char *buff, uint8_t size, char leadingChar)
  {
    if (val >= 0)
      return toStr((uint32_t)val, buff, size, leadingChar) ;
      
    val = -val ;
    char *v = toStr((uint32_t)val, buff+1, size-1) ;
    if (!v) // overflow
    {
      *buff = '*' ;
      return nullptr ;
    }

    v -= 1 ;
    *v = '-' ;

    if (!leadingChar)
      return v ;

    for (char *z = buff ; z < v ; ++z)
      *z = leadingChar ;

    return buff ;
  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
