////////////////////////////////////////////////////////////////////////////////
// gpio.cpp
////////////////////////////////////////////////////////////////////////////////

extern "C"
{
#include "gd32vf103.h"
}

#include "gpio.h"

namespace RV
{
  namespace GD32VF103
  {
    Gpio::Gpio(rcu_periph_enum rcuGpio, uint32_t gpio, uint32_t pin)
      : _rcuGpio{rcuGpio}, _gpio{gpio}, _pin{pin}
    {
    }

    void Gpio::setup(Gpio::Mode mode)
    {
      rcu_periph_clock_enable(_rcuGpio) ;
      gpio_init(_gpio, (uint32_t)mode, GPIO_OSPEED_50MHZ, _pin) ;
    }

    void Gpio::set(bool value)
    {
      if (value)
        GPIO_BOP(_gpio) = _pin ; // GPIO port bit operation register
      else
        GPIO_BC (_gpio) = _pin ; // GPIO bit clear register
    }

    void Gpio::high()
    {
      GPIO_BOP(_gpio) = _pin ; // GPIO port bit operation register
    }
    
    void Gpio::low()
    {
      GPIO_BC (_gpio) = _pin ; // GPIO bit clear register      
    }
      
    bool Gpio::get()
    {
      return gpio_input_bit_get(_gpio, _pin) ;
    }

    Gpio& Gpio::gpioA0()  { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_0)  ; return *gpio ; }
    Gpio& Gpio::gpioA1()  { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_1)  ; return *gpio ; }
    Gpio& Gpio::gpioA2()  { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_2)  ; return *gpio ; }
    Gpio& Gpio::gpioA3()  { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_3)  ; return *gpio ; }
    Gpio& Gpio::gpioA4()  { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_4)  ; return *gpio ; }
    Gpio& Gpio::gpioA5()  { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_5)  ; return *gpio ; }
    Gpio& Gpio::gpioA6()  { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_6)  ; return *gpio ; }
    Gpio& Gpio::gpioA7()  { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_7)  ; return *gpio ; }
    Gpio& Gpio::gpioA8()  { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_8)  ; return *gpio ; }
    Gpio& Gpio::gpioA9()  { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_9)  ; return *gpio ; }
    Gpio& Gpio::gpioA10() { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_10) ; return *gpio ; }
    Gpio& Gpio::gpioA11() { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_11) ; return *gpio ; }
    Gpio& Gpio::gpioA12() { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_12) ; return *gpio ; }
    Gpio& Gpio::gpioA13() { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_13) ; return *gpio ; }
    Gpio& Gpio::gpioA14() { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_14) ; return *gpio ; }
    Gpio& Gpio::gpioA15() { static Gpio *gpio = new Gpio(RCU_GPIOA, GPIOA, GPIO_PIN_15) ; return *gpio ; }
    
    Gpio& Gpio::gpioB0()  { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_0)  ; return *gpio ; }
    Gpio& Gpio::gpioB1()  { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_1)  ; return *gpio ; }
    Gpio& Gpio::gpioB2()  { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_2)  ; return *gpio ; }
    Gpio& Gpio::gpioB3()  { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_3)  ; return *gpio ; }
    Gpio& Gpio::gpioB4()  { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_4)  ; return *gpio ; }
    Gpio& Gpio::gpioB5()  { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_5)  ; return *gpio ; }
    Gpio& Gpio::gpioB6()  { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_6)  ; return *gpio ; }
    Gpio& Gpio::gpioB7()  { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_7)  ; return *gpio ; }
    Gpio& Gpio::gpioB8()  { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_8)  ; return *gpio ; }
    Gpio& Gpio::gpioB9()  { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_9)  ; return *gpio ; }
    Gpio& Gpio::gpioB10() { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_10) ; return *gpio ; }
    Gpio& Gpio::gpioB11() { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_11) ; return *gpio ; }
    Gpio& Gpio::gpioB12() { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_12) ; return *gpio ; }
    Gpio& Gpio::gpioB13() { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_13) ; return *gpio ; }
    Gpio& Gpio::gpioB14() { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_14) ; return *gpio ; }
    Gpio& Gpio::gpioB15() { static Gpio *gpio = new Gpio(RCU_GPIOB, GPIOB, GPIO_PIN_15) ; return *gpio ; }
    
    Gpio& Gpio::gpioC0()  { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_0)  ; return *gpio ; }
    Gpio& Gpio::gpioC1()  { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_1)  ; return *gpio ; }
    Gpio& Gpio::gpioC2()  { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_2)  ; return *gpio ; }
    Gpio& Gpio::gpioC3()  { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_3)  ; return *gpio ; }
    Gpio& Gpio::gpioC4()  { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_4)  ; return *gpio ; }
    Gpio& Gpio::gpioC5()  { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_5)  ; return *gpio ; }
    Gpio& Gpio::gpioC6()  { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_6)  ; return *gpio ; }
    Gpio& Gpio::gpioC7()  { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_7)  ; return *gpio ; }
    Gpio& Gpio::gpioC8()  { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_8)  ; return *gpio ; }
    Gpio& Gpio::gpioC9()  { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_9)  ; return *gpio ; }
    Gpio& Gpio::gpioC10() { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_10) ; return *gpio ; }
    Gpio& Gpio::gpioC11() { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_11) ; return *gpio ; }
    Gpio& Gpio::gpioC12() { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_12) ; return *gpio ; }
    Gpio& Gpio::gpioC13() { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_13) ; return *gpio ; }
    Gpio& Gpio::gpioC14() { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_14) ; return *gpio ; }
    Gpio& Gpio::gpioC15() { static Gpio *gpio = new Gpio(RCU_GPIOC, GPIOC, GPIO_PIN_15) ; return *gpio ; }
    
  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
