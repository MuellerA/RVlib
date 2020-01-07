////////////////////////////////////////////////////////////////////////////////
// led.cpp
////////////////////////////////////////////////////////////////////////////////

extern "C"
{
#include "gd32vf103.h"
}

#include "led.h"

namespace RV
{
  namespace Longan
  {
    RgbLed::Led::Led(Gpio &gpio) : _gpio{gpio}
    {
    }

    void RgbLed::Led::on()  const { _gpio.low()  ; }
    void RgbLed::Led::off() const { _gpio.high() ; }

    const RgbLed::Led &RgbLed::r() { return _r ; }
    const RgbLed::Led &RgbLed::g() { return _g ; }
    const RgbLed::Led &RgbLed::b() { return _b ; }

    RgbLed::RgbLed()
    {
    } ;

    void RgbLed::setup()
    {
      _r._gpio.setup(Gpio::Mode::OUT_PP) ;
      _g._gpio.setup(Gpio::Mode::OUT_PP) ;
      _b._gpio.setup(Gpio::Mode::OUT_PP) ;

      _r.off() ;
      _g.off() ;
      _b.off() ;
    }

    void RgbLed::rgb(bool r, bool g, bool b)
    {
      if (r) _r.on() ; else _r.off() ;
      if (g) _g.on() ; else _g.off() ;
      if (b) _b.on() ; else _b.off() ;
    }

    void RgbLed::red()     { _r.on()  ; _g.off() ; _b.off() ; }
    void RgbLed::green()   { _r.off() ; _g.on()  ; _b.off() ; }
    void RgbLed::blue()    { _r.off() ; _g.off() ; _b.on()  ; }

    void RgbLed::yellow()  { _r.on()  ; _g.on()  ; _b.off() ; }
    void RgbLed::magenta() { _r.on()  ; _g.off() ; _b.on()  ; }
    void RgbLed::cyan()    { _r.off() ; _g.on()  ; _b.on()  ; }

    void RgbLed::white()   { _r.on()  ; _g.on()  ; _b.on()  ; }
    void RgbLed::black()   { _r.off() ; _g.off() ; _b.off() ; }
    
    RgbLed& RgbLed::rgbLed()
    {
      static RgbLed *rgbLed = new RgbLed() ;
      return *rgbLed ;
    }
    
  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
