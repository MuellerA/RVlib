////////////////////////////////////////////////////////////////////////////////
// led.h
////////////////////////////////////////////////////////////////////////////////
//
// uses A1, A2, C13
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GD32VF103/gpio.h"
using ::RV::GD32VF103::Gpio ;

namespace RV
{
  namespace Longan
  {

    class RgbLed
    {
    public:

      class Led
      {
        friend class RgbLed ;
        Led(Gpio &gpio) ;

      public:
        void on()  const ;
        void off() const ;
      private:
        Gpio &_gpio ;
      } ;

      const Led &r() ;
      const Led &g() ;
      const Led &b() ;
    
      void setup() ;
      void rgb(bool r, bool g, bool b) ;
      
      void red()     ;
      void green()   ;
      void blue()    ;

      void yellow()  ;
      void magenta() ;
      void cyan()    ;

      void white()   ;
      void black()   ;

      static RgbLed& rgbLed() ;
      
    private:
      RgbLed() ;
  
      Led _r{Gpio::gpioC13()} ;
      Led _g{Gpio::gpioA1()} ;
      Led _b{Gpio::gpioA2()} ;
    } ;

  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
