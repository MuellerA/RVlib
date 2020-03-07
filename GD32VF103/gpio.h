////////////////////////////////////////////////////////////////////////////////
// gpio.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>

extern "C"
{
  void EXTI0_IRQHandler() ;
  void EXTI1_IRQHandler() ;
  void EXTI2_IRQHandler() ;
  void EXTI3_IRQHandler() ;
  void EXTI4_IRQHandler() ;
  void EXTI5_9_IRQHandler() ;
  void EXTI10_15_IRQHandler() ;
}

namespace RV
{
  namespace GD32VF103
  {

    class Gpio
    {
    private:
      Gpio(rcu_periph_enum rcuGpio, uint32_t gpio, uint32_t pin) ;
      Gpio(const Gpio&) = delete ;
      
    public:
      enum class Mode
        {
         IN_AN  = GPIO_MODE_AIN,          // analog input
         IN_FL  = GPIO_MODE_IN_FLOATING,  // floating input
         IN_PD  = GPIO_MODE_IPD,          // pull down input
         IN_PU  = GPIO_MODE_IPU,          // pull up input
         OUT_OD = GPIO_MODE_OUT_OD,       // open drain output
         OUT_PP = GPIO_MODE_OUT_PP,       // push pull output
         AF_OD  = GPIO_MODE_AF_OD,        // open drain AF
         AF_PP  = GPIO_MODE_AF_PP,        // push pull AF
        } ;
      void setup(Mode mode) ;

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
    } ;

    ////////////////////////////////////////////////////////////////////////////////
    
    class GpioIrq
    {
    private:
      friend void ::EXTI0_IRQHandler() ;
      friend void ::EXTI1_IRQHandler() ;
      friend void ::EXTI2_IRQHandler() ;
      friend void ::EXTI3_IRQHandler() ;
      friend void ::EXTI4_IRQHandler() ;
      friend void ::EXTI5_9_IRQHandler() ;
      friend void ::EXTI10_15_IRQHandler() ;
      
      GpioIrq(rcu_periph_enum rcuGpio, uint32_t gpio, uint32_t pin, uint32_t eclicSource,
              uint8_t extiSourcePort, uint8_t extiSourcePin, exti_line_enum extiLine) ;
      GpioIrq(const GpioIrq&) = delete ;

    public:
      using Handler = std::function<void(bool rising)> ;

      enum class Mode
        {
         IN_FL  = GPIO_MODE_IN_FLOATING,  // floating input
         IN_PD  = GPIO_MODE_IPD,          // pull down input
         IN_PU  = GPIO_MODE_IPU,          // pull up input
        } ;

      void setup(Mode mode, Handler handler) ;

      volatile bool pressed() ;
      void irqDisable() ;
      void irqEnable() ;

      static GpioIrq& gpioA0() ;  // static GpioIrq& gpioB0() ;   static GpioIrq& gpioC0() ; 
      static GpioIrq& gpioA1() ;  // static GpioIrq& gpioB1() ;   static GpioIrq& gpioC1() ; 
      static GpioIrq& gpioA2() ;  // static GpioIrq& gpioB2() ;   static GpioIrq& gpioC2() ; 
      static GpioIrq& gpioA3() ;  // static GpioIrq& gpioB3() ;   static GpioIrq& gpioC3() ; 
      static GpioIrq& gpioA4() ;  // static GpioIrq& gpioB4() ;   static GpioIrq& gpioC4() ; 
      static GpioIrq& gpioA5() ;  // static GpioIrq& gpioB5() ;   static GpioIrq& gpioC5() ; 
      static GpioIrq& gpioA6() ;  // static GpioIrq& gpioB6() ;   static GpioIrq& gpioC6() ; 
      static GpioIrq& gpioA7() ;  // static GpioIrq& gpioB7() ;   static GpioIrq& gpioC7() ; 
      static GpioIrq& gpioA8() ;  // static GpioIrq& gpioB8() ;   static GpioIrq& gpioC8() ; 
      static GpioIrq& gpioA9() ;  // static GpioIrq& gpioB9() ;   static GpioIrq& gpioC9() ; 
      static GpioIrq& gpioA10() ; // static GpioIrq& gpioB10() ;  static GpioIrq& gpioC10() ;
      static GpioIrq& gpioA11() ; // static GpioIrq& gpioB11() ;  static GpioIrq& gpioC11() ;
      static GpioIrq& gpioA12() ; // static GpioIrq& gpioB12() ;  static GpioIrq& gpioC12() ;
      static GpioIrq& gpioA13() ; // static GpioIrq& gpioB13() ;  static GpioIrq& gpioC13() ;
      static GpioIrq& gpioA14() ; // static GpioIrq& gpioB14() ;  static GpioIrq& gpioC14() ;
      static GpioIrq& gpioA15() ; // static GpioIrq& gpioB15() ;  static GpioIrq& gpioC15() ;

    private:

      void irqHandler() ;
      
    private:
      rcu_periph_enum _rcuGpio ;
      uint32_t _gpio ;
      uint32_t _pin ;
      uint32_t _eclicSource ;
      uint8_t _extiSourcePort ;
      uint8_t _extiSourcePin ;
      exti_line_enum _extiLine ;
      Handler _handler ;

      volatile bool _pressed ;
    } ;   
    
  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
