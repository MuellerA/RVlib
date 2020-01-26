////////////////////////////////////////////////////////////////////////////////
// toStr.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

namespace RV
{
  char* toStr(uint32_t val, char *buff, uint8_t size, char leadingChar = 0 , bool hex = false) ;
  char* toStr( int32_t val, char *buff, uint8_t size, char leadingChar = 0) ;

  inline char* toStr(uint16_t val, char *buff, uint8_t size, char leadingChar = 0 , bool hex = false)
  { return toStr((uint32_t)val, buff, size, leadingChar, hex) ; }
  inline char* toStr( int16_t val, char *buff, uint8_t size, char leadingChar = 0)
  { return toStr(( int32_t)val, buff, size, leadingChar) ; }
    
  inline char* toStr(uint8_t val, char *buff, uint8_t size, char leadingChar = 0 , bool hex = false)
  { return toStr((uint32_t)val, buff, size, leadingChar, hex) ; }
  inline char* toStr( int8_t val, char *buff, uint8_t size, char leadingChar = 0)
  { return toStr(( int32_t)val, buff, size, leadingChar) ; }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
