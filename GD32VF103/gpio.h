////////////////////////////////////////////////////////////////////////////////
// gpio.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace RV
{
  namespace GD32VF103
  {

    class Gpio
    {
      Gpio(rcu_periph_enum rcuGpio, uint32_t gpio, uint32_t pin) ;

    public:
      void setup(uint32_t mode) ;

      void set(bool value) ;
      void high() ;
      void low() ;
      
      bool get() ;

      static Gpio& gpioA0() ;   static Gpio& gpioB0() ;   static Gpio& gpioC0() ; 
      static Gpio& gpioA1() ;   static Gpio& gpioB1() ;   static Gpio& gpioC1() ; 
      static Gpio& gpioA2() ;   static Gpio& gpioB2() ;   static Gpio& gpioC2() ; 
      static Gpio& gpioA3() ;   static Gpio& gpioB3() ;   static Gpio& gpioC3() ; 
      static Gpio& gpioA4() ;   static Gpio& gpioB4() ;   static Gpio& gpioC4() ; 
      static Gpio& gpioA5() ;   static Gpio& gpioB5() ;   static Gpio& gpioC5() ; 
      static Gpio& gpioA6() ;   static Gpio& gpioB6() ;   static Gpio& gpioC6() ; 
      static Gpio& gpioA7() ;   static Gpio& gpioB7() ;   static Gpio& gpioC7() ; 
      static Gpio& gpioA8() ;   static Gpio& gpioB8() ;   static Gpio& gpioC8() ; 
      static Gpio& gpioA9() ;   static Gpio& gpioB9() ;   static Gpio& gpioC9() ; 
      static Gpio& gpioA10() ;  static Gpio& gpioB10() ;  static Gpio& gpioC10() ;
      static Gpio& gpioA11() ;  static Gpio& gpioB11() ;  static Gpio& gpioC11() ;
      static Gpio& gpioA12() ;  static Gpio& gpioB12() ;  static Gpio& gpioC12() ;
      static Gpio& gpioA13() ;  static Gpio& gpioB13() ;  static Gpio& gpioC13() ;
      static Gpio& gpioA14() ;  static Gpio& gpioB14() ;  static Gpio& gpioC14() ;
      static Gpio& gpioA15() ;  static Gpio& gpioB15() ;  static Gpio& gpioC15() ;

    private:
      rcu_periph_enum _rcuGpio ;
      uint32_t _gpio ;
      uint32_t _pin ;
      uint32_t _mode ;
    } ;

  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
