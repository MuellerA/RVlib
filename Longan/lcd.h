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
#include "GD32VF103/time.h"

#include "lib/Adafruit-GFX-Library/gfxfont.h"
#include <functional>

namespace RV
{
  namespace Longan
  {
    class Lcd ;
    class LcdCmdData ;

    class LcdArea
    {
    public:
      LcdArea(Lcd &lcd, uint32_t xMin = 0, uint32_t xSize = 160, uint32_t yMin = 0, uint32_t ySize = 80, const GFXfont *gfxFont = nullptr, uint32_t fgCol = 0xffffff, uint32_t bgCol = 0x000000) ;

      void area(uint32_t xMin, uint32_t _xSize, uint32_t yMin, uint32_t ySize) ;
      void font(const GFXfont *font) ;
      void color(uint32_t fgCol, uint32_t bgCol) ;
      void pos(uint32_t x, uint32_t y) ;
      void txtPos(uint32_t row, uint32_t col = 0) ;

      void clear() ;
      void clear(uint32_t rgb) ;
      void fill(uint8_t xMin, uint8_t xSize, uint8_t yMin, uint8_t ySize, uint32_t rgb = 0x000000) ;
      void put(char ch) ;
      void put(const char *str) ;
      void put(const char *str, uint32_t size) ;
      void put(uint32_t val, uint8_t size = 0, char leadingChar = 0 , bool hex = false) ;
      void put( int32_t val, uint8_t size = 0, char leadingChar = 0) ;
      void put(uint16_t val, uint8_t size = 0, char leadingChar = 0 , bool hex = false)
      { put((uint32_t) val, size, leadingChar, hex) ; }
      void put( int16_t val, uint8_t size = 0, char leadingChar = 0)
      { put(( int32_t) val, size, leadingChar) ; }
      void put(uint8_t val, uint8_t size = 0, char leadingChar = 0 , bool hex = false)
      { put((uint32_t) val, size, leadingChar, hex) ; }
      void put( int8_t val, uint8_t size = 0, char leadingChar = 0)
      { put(( int32_t) val, size, leadingChar) ; }

      uint32_t x() const ;
      uint32_t y() const ;
      uint32_t fgCol() const ;
      uint32_t bgCol() const ;
      
      uint16_t xMin() const ;
      uint16_t xMax() const ;
      uint16_t xSize() const ;
      uint16_t yMin() const ;
      uint16_t yMax() const ;
      uint16_t ySize() const ;

      const GFXfont* font() const ;
      uint8_t baseLineOffset() const ;

    private:
      Lcd &_lcd ;

      uint32_t _x ; // cursor pos
      uint32_t _y ;
      uint32_t _fgCol ;
      uint32_t _bgCol ;
      
      uint16_t _xMin ;
      uint16_t _xMax ;
      uint16_t _xSize ;
      uint16_t _yMin ;
      uint16_t _yMax ;
      uint16_t _ySize ;

      const GFXfont *_font ;
      uint8_t _baseLineOffset ;
    } ;
    
    class Lcd : public LcdArea
    {
      friend class LcdArea ;
      
    private:
      Lcd() ;
      Lcd(const Lcd&) = delete ;
      
    public:
      static Lcd& lcd() ;

      void setup() ;

      void heartbeat() ; // animation in upper right corner
      
      void off() ;
      void on() ;
      void sleepIn() ; // takes 120ms!
      void sleepOut() ; // takes 120ms!
      
    private:
      void absFill(uint8_t xMin, uint8_t xSize, uint8_t yMin, uint8_t ySize, uint32_t rgb = 0x000000) ;
      void absFill(uint8_t xMin, uint8_t xSize, uint8_t yMin, uint8_t ySize, std::function<uint32_t()> rgbCb) ;
      
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

      // heartbeat
      bool    _hbDir{false} ; // direction
      uint8_t _hbPos{0} ; // value
      ::RV::GD32VF103::TickTimer _hbTimer{250, true} ;
    } ;

  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
