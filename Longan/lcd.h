////////////////////////////////////////////////////////////////////////////////
// lcd.h
////////////////////////////////////////////////////////////////////////////////
//
// uses SPI0[A5,A6,A7], B0, B1, B2
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GD32VF103/gpio.h"
#include "GD32VF103/spi.h"

namespace RV
{
  namespace Longan
  {
    
    class LcdCmdData ;

    class Lcd
    {
      Lcd() ;
    public:
      static Lcd& lcd() ;
      
      void setup(const uint8_t *font, uint8_t fontHeight, uint8_t fontWidth) ;
      void fill(uint8_t xMin, uint8_t xMax, uint8_t yMin, uint8_t yMax, uint32_t rgb = 0x000000) ;
      void put(char ch) ;
      void put(const char *str) ;
      void txtArea(uint8_t xMin, uint8_t xMax, uint8_t yMin, uint8_t yMax) ;
      void txtPos(uint8_t row = 0, uint8_t col = 0) ;
      void txtFg(uint32_t rgb) ;
      void txtBg(uint32_t rgb) ;

    private:
      void rstHi() ;
      void rstLo() ;
      void rsHi()  ;
      void rsLo()  ;
      void csHi()  ;
      void csLo()  ;

      void cmd(uint8_t cmd) ;
      void data(uint8_t data) ;
      void cmd(const LcdCmdData &cmdData) ;

      ::RV::GD32VF103::Spi &_spi ;
      ::RV::GD32VF103::Gpio &_pinRst ;
      ::RV::GD32VF103::Gpio &_pinRs ;
      ::RV::GD32VF103::Gpio &_pinCs ;

      uint32_t _txtAreaXmin ; // area in which text is printed
      uint32_t _txtAreaXmax ;
      uint32_t _txtAreaYmin ;
      uint32_t _txtAreaYmax ;
      uint32_t _txtPosX ; // pos at which next char is inserted
      uint32_t _txtPosY ;
      const uint8_t  *_font ; // ' ' .. '~'
      uint8_t  _fontHeight ; // fixed font only
      uint8_t  _fontWidth ;
      uint32_t _txtFg ;
      uint32_t _txtBg ;
    } ;

  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
