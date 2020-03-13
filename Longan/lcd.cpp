////////////////////////////////////////////////////////////////////////////////
// lcd.cpp
////////////////////////////////////////////////////////////////////////////////

extern "C"
{
#include "gd32vf103.h"
}

#include "lcd.h"
#include "toStr.h"
#include "fonts.h"

using ::RV::GD32VF103::TickTimer ;
using ::RV::GD32VF103::Spi ;
using ::RV::GD32VF103::Gpio ;

namespace RV
{
  namespace Longan
  {
    class LcdCmdData
    {
    public:
      uint8_t _cmd ;
      uint8_t _size ;
      uint8_t _data[16] ; // max size
    } ;

    ////////////////////////////////////////////////////////////////////////////////
    
    LcdArea::LcdArea(Lcd &lcd, uint32_t xMin, uint32_t xSize, uint32_t yMin, uint32_t ySize, const GFXfont *gfxFont, uint32_t fgCol, uint32_t bgCol)
      : _lcd{lcd}
    {
      area(xMin, xSize, yMin, ySize) ;
      font(gfxFont) ;
      color(fgCol, bgCol) ;
    }

    void LcdArea::area(uint32_t xMin, uint32_t xSize, uint32_t yMin, uint32_t ySize)
    {
      _xMin = xMin ;
      _xMax = xMin + xSize - 1 ;
      _xSize = xSize ;

      _yMin = yMin ;
      _yMax = yMin + ySize - 1 ;
      _ySize = ySize ;
    }
    
    void LcdArea::font(const GFXfont *gfxFont)
    {
      _font = gfxFont ? gfxFont : &Roboto_Light7pt7b ;
      GFXglyph *glyph = _font->glyph ;
      int8_t yOffset = 0 ;
      for (uint16_t iGlyph = _font->first ; iGlyph <= _font->last ; ++iGlyph)
      {
        if (glyph->yOffset < yOffset)
          yOffset = glyph->yOffset ;
        glyph += 1 ;
      }
      _baseLineOffset = -yOffset ;

      _x = 0, _y = _baseLineOffset ;
    }

    void LcdArea::color(uint32_t fgCol, uint32_t bgCol)
    {
      _fgCol = fgCol ;
      _bgCol = bgCol ;
    }

    void LcdArea::pos(uint32_t x, uint32_t y)
    {
      _x = x ; _y = y ;
    }

    void LcdArea::txtPos(uint32_t row, uint32_t col)
    {
      if (!col)
      {
        _x = 0 ;
      }
      else if ((_font->first <= ' ') && (' ' <= _font->last))
      {
        const GFXglyph *sp = _font->glyph + ' ' - _font->first ;
        _x = col * sp->xAdvance ;
      }
      else
      {
        _x = col * _font->glyph->xAdvance ;
      }

      _y = row * _font->yAdvance + _baseLineOffset ;
    }
    
    void LcdArea::clear()
    {
      clear(_bgCol) ;
    }
    void LcdArea::clear(uint32_t rgb)
    {
      fill(0, _xSize, 0, _ySize, rgb) ;
      _x = 0 ;
      _y = _baseLineOffset ;
    }
    
    void LcdArea::fill(uint8_t xMin, uint8_t xSize, uint8_t yMin, uint8_t ySize, uint32_t rgb)
    {
      if (xSize > (_xSize - xMin)) xSize = _xSize - xMin ;
      if (ySize > (_ySize - yMin)) ySize = _ySize - yMin ;
      
      _lcd.absFill(_xMin + xMin, xSize, _yMin + yMin, ySize, rgb) ;
    }

   void LcdArea::put(char ch)
    {
      if ((_font->first <= ch) && (ch <= _font->last))
      {
        const GFXglyph *glyph = _font->glyph + ch - _font->first ;
        uint32_t xSize = glyph->xAdvance ;
        uint32_t ySize = _font->yAdvance ;
        
        if ((xSize > _xSize) ||
            (ySize > _ySize))
          return ;

        if (_x + xSize > _xSize)
        {
          _x = 0 ;
          _y += ySize ;
        }
        if ((_y + (ySize - _baseLineOffset)) > _ySize)
        {
          _y = _baseLineOffset ;
        }

        uint32_t absX = _x + _xMin ;
        uint32_t absY = _y + _yMin - _baseLineOffset; ;

        _lcd.absFill(absX, xSize, absY, ySize, _bgCol) ;

        absX = _x + _xMin + glyph->xOffset ;
        absY = _y + _yMin + glyph->yOffset ;
        uint32_t absXSize = glyph->width ;
        uint32_t absYSize = glyph->height ;

        uint32_t bit = 0 ;
        uint8_t  *data = _font->bitmap + glyph->bitmapOffset ;
        
        _lcd.absFill(absX, absXSize, absY, absYSize, [&bit, data, this]()->uint32_t
                                                     {
                                                       uint32_t byte = bit / 8 ;
                                                       uint8_t  mask = 0x80 >> (bit % 8) ;
                                                       bit += 1 ;
                                                       return (data[byte] & mask) ? _fgCol : _bgCol ;
                                                     }) ;
        _x += xSize ;
      }
      else
      {
        if (ch == 0x0a) // LF
        {
          uint32_t ySize = _font->yAdvance ;
          
          _x = 0 ;
          _y += ySize ;
          if ((_y + (ySize - _baseLineOffset)) > _ySize)
          {
            _y = _baseLineOffset ;
          }
          return ;
        }
        if (ch == 0x0d) // CR
        {
          _x = 0 ;
          return ;
        }
        if (ch == 0x0c) // FF
        {
          _x = 0 ;
          _y = _baseLineOffset ;
          clear() ;
          return ;
        }
      }
    }

    void LcdArea::put(const char *str)
    {
      while (*str)
        put(*(str++)) ;
    }

    void LcdArea::put(const char *str, uint32_t size)
    {
      const char *eStr = str + size ;
      while (str < eStr)
        put(*(str++)) ;
    }

    void LcdArea::put(const char *str, uint16_t x0, uint16_t y0, int8_t hAlign, int8_t vAlign)
    {
      int16_t x{0} ;
      int16_t y{0} ;
      int16_t xMin{0} ;
      int16_t xMax{0} ;
      int16_t yMin{0} ;
      int16_t yMax{0} ;
      
      // calculate widht/height
      for (const char *c = str ; *c ; ++c)
      {
        char ch = *c ;
        if ((_font->first <= ch) && (ch <= _font->last))
        {
          const GFXglyph *glyph = _font->glyph + ch - _font->first ;
          if (xMin > (x + glyph->xOffset                    )) xMin = (x + glyph->xOffset                    ) ;
          if (xMax < (x + glyph->xOffset + glyph->width  - 1)) xMax = (x + glyph->xOffset + glyph->width  - 1) ;
          if (yMin > (y + glyph->yOffset                    )) yMin = (y + glyph->yOffset                    ) ;
          if (yMax < (y + glyph->yOffset + glyph->height - 1)) yMax = (y + glyph->yOffset + glyph->height - 1) ;
          x += glyph->xAdvance ;
        }
      }

      struct DstBuff
      {
        uint16_t _width{0} ;      // width in pixel
        uint16_t _widthBytes{0} ; // width in bytes
        uint16_t _height{0} ;
        std::vector<uint8_t> _buff{} ; // pixed data
      } dstBuff ;
      
      // check size
      if ((xMin >= xMax) || (yMin >= yMax))
        return ;
      dstBuff._width  = xMax - xMin + 1 ;
      dstBuff._height = yMax - yMin + 1 ;
      if ((dstBuff._width > 160) || (dstBuff._height > 80))
        return ;

      // alloc pixel buffer
      dstBuff._widthBytes = (dstBuff._width+7) / 8 ;
      dstBuff._buff.resize(dstBuff._widthBytes * dstBuff._height, 0x00) ;

      // fill pixel buffer
      x = -xMin ;
      y = -yMin ;
      for (const char *c = str ; *c ; ++c)
      {
        char ch = *c ;
        if ((_font->first <= ch) && (ch <= _font->last))
        {
          const GFXglyph *glyph = _font->glyph + ch - _font->first ;

          uint8_t *srcByte = _font->bitmap + glyph->bitmapOffset ;
          uint8_t srcMask = 0x80 ;

          int16_t dstXUL = x + glyph->xOffset ; // upper left
          int16_t dstYUL = y + glyph->yOffset ;
          
          for (uint8_t iY = 0 ; iY < glyph->height ; ++iY)
            for (uint8_t iX = 0 ; iX < glyph->width ; ++iX)
            {
              if (*srcByte & srcMask)
              {
                uint16_t dstByte = dstBuff._widthBytes * (dstYUL+iY) + (dstXUL+iX)/8 ;
                uint16_t dstMask = 0x80 >> ((dstXUL+iX)%8) ;
                dstBuff._buff[dstByte] |= dstMask ;
              }

              srcMask >>= 1 ;
              if (!srcMask)
              {
                srcByte += 1 ;
                srcMask = 0x80 ;
              }
            }
          
          x += glyph->xAdvance ;
        }
      }

                    
      // alignment
      int16_t x1, xS, y1, yS ;
      if      (hAlign < 0) x1 = (int16_t)x0 ;
      else if (hAlign > 0) x1 = (int16_t)x0 - (int16_t)dstBuff._width ;
      else                 x1 = (int16_t)x0 - (int16_t)(dstBuff._width / 2) ;
      xS = x1 + dstBuff._width ;
      
      if      (vAlign < 0) y1 = (int16_t)y0 ;
      else if (vAlign > 0) y1 = (int16_t)y0 - (int16_t)dstBuff._height ;
      else                 y1 = (int16_t)y0 - (int16_t)(dstBuff._height / 2) ;
      yS = y1 + dstBuff._height ;
      
      // copy to lcd
      struct
      {
        uint16_t _xMin ; // pixels to transmit (buffer might be clipped)
        uint16_t _xMax ;
        uint16_t _yMin ;
        uint16_t _yMax ;
        uint16_t _x ;
        uint16_t _y ;
        uint32_t _fgCol ;
        uint32_t _bgCol ;
      } cpInfo ;

      cpInfo._xMin = (x1 < 0) ? -x1 : 0 ;
      cpInfo._xMax = ((xS > _xSize) ? dstBuff._width - (xS - _xSize) : dstBuff._width) - 1 ;
      cpInfo._yMin = (y1 < 0) ? -y1 : 0 ;
      cpInfo._yMax = ((yS > _ySize) ? dstBuff._height - (yS - _ySize) : dstBuff._height) - 1 ;
      cpInfo._x = cpInfo._xMin ;
      cpInfo._y = cpInfo._yMin ;
      cpInfo._fgCol = _fgCol ;
      cpInfo._bgCol = _bgCol ;

      uint16_t absX, absXSize, absY, absYSize ;
      absX = ((x1 < 0) ? 0 : x1) + _xMin ;
      absXSize = cpInfo._xMax - cpInfo._xMin + 1 ;
      absY = ((y1 < 0) ? 0 : y1) + _yMin ;
      absYSize = cpInfo._yMax - cpInfo._yMin + 1 ;
      
      _lcd.absFill(absX, absXSize, absY, absYSize, [&dstBuff, &cpInfo]()->uint32_t
                                                   {
                                                     uint16_t byte = dstBuff._widthBytes * cpInfo._y + (cpInfo._x)/8 ;
                                                     uint16_t mask = 0x80 >> (cpInfo._x%8) ;

                                                     cpInfo._x += 1 ;
                                                     if (cpInfo._x > cpInfo._xMax)
                                                     {
                                                       cpInfo._x =  cpInfo._xMin ;
                                                       cpInfo._y += 1 ;
                                                     }
                                                     return (dstBuff._buff[byte] & mask) ? cpInfo._fgCol : cpInfo._bgCol ;
                                                   }) ;
    }    

    void LcdArea::put(uint32_t val, uint8_t fmtSize, char leadingChar , bool hex)
    {
      uint8_t size = fmtSize ? fmtSize : 16 ;
      if (fmtSize && !leadingChar)
        leadingChar = ' ' ;
      char buff[size] ;
      char *str = ::RV::toStr(val, buff, size, leadingChar, hex) ;
      if (str)
        put(str, fmtSize ? fmtSize : buff+size-str) ;
      else
        put(buff, fmtSize ? fmtSize : 1) ;
    }
    
    void LcdArea::put( int32_t val, uint8_t fmtSize, char leadingChar)
    {
      uint8_t size = fmtSize ? fmtSize : 16 ;
      if (fmtSize && !leadingChar)
        leadingChar = ' ' ;
      char buff[size] ;
      char *str = ::RV::toStr(val, buff, size, leadingChar) ;
      if (str)
        put(str, fmtSize ? fmtSize : buff+size-str) ;
      else
        put(buff, fmtSize ? fmtSize : 1) ;
    }
    
    uint32_t LcdArea::x() const { return _x ; }
    uint32_t LcdArea::y() const { return _y ; }
    uint32_t LcdArea::fgCol() const { return _fgCol ; }
    uint32_t LcdArea::bgCol() const { return _bgCol ; }
      
    uint16_t LcdArea::xMin() const { return _xMin ; }
    uint16_t LcdArea::xMax() const { return _xMax ; }
    uint16_t LcdArea::xSize() const { return _xSize ; }
    uint16_t LcdArea::yMin() const { return _yMin ; }
    uint16_t LcdArea::yMax() const { return _yMax ; }
    uint16_t LcdArea::ySize() const { return _ySize ; }

    const GFXfont* LcdArea::font() const { return _font ; }
    uint8_t  LcdArea::baseLineOffset() const { return _baseLineOffset ; }

    ////////////////////////////////////////////////////////////////////////////////
    
    Lcd::Lcd()
      // LCD: B0 RS, B1 RST, B2 CS
      : LcdArea{*this},
        _spi{Spi::spi0()}, _pinRst{Gpio::gpioB1()}, _pinRs{Gpio::gpioB0()}, _pinCs{Gpio::gpioB2()}
    {
    }

    Lcd& Lcd::lcd()
    {
      static Lcd *lcd = new Lcd() ;
      return *lcd ;
    }
    
    void Lcd::setup()
    {
      _spi.setup() ;
      _pinRst.setup(Gpio::Mode::OUT_PP) ;
      _pinRs.setup(Gpio::Mode::OUT_PP) ;
      _pinCs.setup(Gpio::Mode::OUT_PP) ;
  
      csHi() ;

      // HW Reset
      rstHi() ; TickTimer::delayMs( 25) ;
      rstLo() ; TickTimer::delayMs(250) ;
      rstHi() ; TickTimer::delayMs( 25) ;

      // Sleep Out
      sleepOut() ;

      // Display Inversion On
      cmd(0x21) ;
      // Frame Rate Control (In normal mode/ Full colors)
      cmd({0xb1,  3, { 0x05, 0x3a, 0x3a } }) ;
      // Frame Rate Control (In Idle mode/ 8-colors)
      cmd({0xb2,  3, { 0x05, 0x3a, 0x3a } }) ;
      // Frame Rate Control (In Partial mode/ full colors)
      cmd({0xB3,  6, { 0x05, 0x3A, 0x3A, 0x05, 0x3A, 0x3A } }) ;
      // Display Inversion Control
      cmd({0xB4,  1, { 0x03 } }) ;
      // Power Control 1
      cmd({0xC0,  3, { 0x62, 0x02, 0x04} }) ;
      // Power Control 2
      cmd({0xC1,  1, { 0xC0 } }) ;
      // Power Control 3 (in Normal mode/ Full colors)
      cmd({0xC2,  2, { 0x0D, 0x00 } }) ;
      // Power Control 4 (in Idle mode/ 8-colors)
      cmd({0xC3,  2, { 0x8D, 0x6A } }) ;
      // Power Control 5 (in Partial mode/ full-colors)
      cmd({0xC4,  2, { 0x8D, 0xEE } }) ;
      // VCOM Control 1
      cmd({0xC5,  1, { 0x0E } }) ;
      // Gamma (‘+’polarity) Correction Characteristics Setting
      cmd({0xE0, 16, { 0x10, 0x0E, 0x02, 0x03, 0x0E, 0x07, 0x02, 0x07, 0x0A, 0x12, 0x27, 0x37, 0x00, 0x0D, 0x0E, 0x10 } }) ;
      // Gamma ‘-’polarity Correction Characteristics Setting
      cmd({0xE1, 16, { 0x10, 0x0E, 0x03, 0x03, 0x0F, 0x06, 0x02, 0x08, 0x0A, 0x13, 0x26, 0x36, 0x00, 0x0D, 0x0E, 0x10 } }) ;
      // Interface Pixel Format
      cmd({0x3A,  1, { 0x06 } }) ; // 18 bit/pixel
      // Memory Data Access Control
      cmd({0x36,  1, { 0xa8 } }) ; // orientation 08, c8, 78, a8
      // clr
      clear() ;
      // Display On
      on() ;
    }

    void Lcd::absFill(uint8_t xMin, uint8_t xSize, uint8_t yMin, uint8_t ySize, uint32_t rgb)
    {
      if (!xSize || !ySize)
        return ;
      
      // Column Address Set
      cmd({0x2a, 4, { 0x00, (uint8_t)( 1+xMin), 0x00, (uint8_t)( 1+xMin+xSize-1) } }) ; // x-offset  1
      // Row Address Set
      cmd({0x2b, 4, { 0x00, (uint8_t)(26+yMin), 0x00, (uint8_t)(26+yMin+ySize-1) } }) ; // y-offset 26
      // Memory Write
      cmd(0x2c) ;
  
      csLo() ;
      rsHi() ;
      for (uint32_t i = 0 ; i < xSize ; ++i)
      {
        for (uint32_t j = 0 ; j < ySize ; ++j)
        {
          _spi.put(rgb >> 16) ;
          _spi.put(rgb >>  8) ;
          _spi.put(rgb >>  0) ;
        }
      }
      while (_spi.isTransmit()) ;
      csHi() ;
    }

    void Lcd::absFill(uint8_t xMin, uint8_t xSize, uint8_t yMin, uint8_t ySize, std::function<uint32_t()> rgbCb)
    {
      if (!xSize || !ySize)
        return ;
      
      // Column Address Set
      cmd({0x2a, 4, { 0x00, (uint8_t)( 1+xMin), 0x00, (uint8_t)( 1+xMin+xSize-1) } }) ; // x-offset  1
      // Row Address Set
      cmd({0x2b, 4, { 0x00, (uint8_t)(26+yMin), 0x00, (uint8_t)(26+yMin+ySize-1) } }) ; // y-offset 26
      // Memory Write
      cmd(0x2c) ;
  
      csLo() ;
      rsHi() ;
      for (uint32_t i = 0 ; i < xSize ; ++i)
      {
        for (uint32_t j = 0 ; j < ySize ; ++j)
        {
          uint32_t rgb = rgbCb() ;
          _spi.put(rgb >> 16) ;
          _spi.put(rgb >>  8) ;
          _spi.put(rgb >>  0) ;
        }
      }
      while (_spi.isTransmit()) ;
      csHi() ;
    }

    void Lcd::heartbeat()
    {
      static uint8_t g{0} ;

      if (!_hbTimer())
        return ;

      // Column Address Set
      cmd({0x2a, 4, { 0x00, (uint8_t)( 1+154), 0x00, (uint8_t)( 1+159) } }) ; // x-offset  1
      // Row Address Set
      cmd({0x2b, 4, { 0x00, (uint8_t)(26+  0), 0x00, (uint8_t)(26+  2) } }) ; // y-offset 26
      // Memory Write
      cmd(0x2c) ;

      csLo() ;
      rsHi() ;
      
      g = ~g ;
      for (uint8_t y = 0 ; y < 3 ; ++y)
      {
        g = ~g ;
        for (uint8_t x = 0 ; x < 3 ; ++x)
        {
          _spi.put(0x00) ;
          _spi.put(g) ;
          _spi.put(0x00) ;
        }
        g = ~g ;
        for (uint8_t x = 0 ; x < 3 ; ++x)
        {
          _spi.put(0x00) ;
          _spi.put(g) ;
          _spi.put(0x00) ;
        }
      }
      
      while (_spi.isTransmit()) ;
      csHi() ;
    }

    void Lcd::off() { cmd(0x28) ; }
    void Lcd::on()  { cmd(0x29) ; }
    void Lcd::sleepIn()  { cmd(0x10) ; TickTimer::delayMs(120) ; }
    void Lcd::sleepOut() { cmd(0x11) ; TickTimer::delayMs(120) ; }
      
    
    void Lcd::rstHi() { _pinRst.high() ; }
    void Lcd::rstLo() { _pinRst.low()  ; }
    void Lcd::rsHi()  { _pinRs.high()  ; }
    void Lcd::rsLo()  { _pinRs.low()   ; }
    void Lcd::csHi()  { _pinCs.high()  ; }
    void Lcd::csLo()  { _pinCs.low()   ; }

    void Lcd::cmd(uint8_t cmd)
    {
      csLo() ;

      rsLo() ;
      _spi.put(cmd) ;
      while (_spi.isTransmit()) ;
  
      csHi() ;
    }

    void Lcd::data(uint8_t data)
    {
      csLo() ;

      rsHi() ;
      _spi.put(data) ;
      while (_spi.isTransmit()) ;
  
      csHi() ;
    }

    void Lcd::cmd(const LcdCmdData &cmdData)
    {
      csLo() ;

      rsLo() ;
      _spi.put(cmdData._cmd) ;
      while (_spi.isTransmit()) ;

      rsHi() ;
      for (uint8_t i = 0 ; i < cmdData._size ; ++i)
        _spi.put(cmdData._data[i]) ;
      while (_spi.isTransmit()) ;
  
      csHi() ;
    }

  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
