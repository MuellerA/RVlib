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

    ////////////////////////////////////////////////////////////////////////////////

    GpioIrq::GpioIrq(rcu_periph_enum rcuGpio, uint32_t gpio, uint32_t pin, uint32_t eclicSource,
                     uint8_t extiSourcePort, uint8_t extiSourcePin, exti_line_enum extiLine)
      : _rcuGpio{rcuGpio}, _gpio{gpio}, _pin{pin}, _eclicSource{eclicSource},
        _extiSourcePort{extiSourcePort}, _extiSourcePin{extiSourcePin}, _extiLine{extiLine},
        _pressed{false}
    {
    }

    void GpioIrq::setup(GpioIrq::Mode mode, Handler handler)
    {
      _handler = handler ;

      rcu_periph_clock_enable(_rcuGpio) ;
      rcu_periph_clock_enable(RCU_AF);
      
      gpio_init(_gpio, (uint32_t)mode, GPIO_OSPEED_50MHZ, _pin) ;
      eclic_global_interrupt_enable();
      eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
      eclic_irq_enable(_eclicSource, 1, 1);
      gpio_exti_source_select(_extiSourcePort, _extiSourcePin);
      exti_init(_extiLine, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
      exti_interrupt_flag_clear(_extiLine);
    }

    volatile bool GpioIrq::pressed()
    {
      irqDisable() ;
      bool p = _pressed ;
      _pressed = false ;
      irqEnable() ;
      return p ;
    }

    void GpioIrq::irqDisable()
    {
      eclic_irq_disable(_eclicSource) ;
    }

    void GpioIrq::irqEnable()
    {
      eclic_irq_enable(_eclicSource, 1, 1) ;
    }
    
    void GpioIrq::irqHandler()
    {
      bool p = gpio_input_bit_get(_gpio, _pin) ;
      _pressed |= p ;
      if (_handler) _handler(p) ;
    }    

    GpioIrq& GpioIrq::gpioA0()  { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_0 , EXTI0_IRQn    , GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_0 , EXTI_0 } ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA1()  { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_1 , EXTI1_IRQn    , GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_1 , EXTI_1 } ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA2()  { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_2 , EXTI2_IRQn    , GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_2 , EXTI_2 } ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA3()  { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_3 , EXTI3_IRQn    , GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_3 , EXTI_3 } ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA4()  { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_4 , EXTI5_9_IRQn  , GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_4 , EXTI_4 } ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA5()  { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_5 , EXTI5_9_IRQn  , GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_5 , EXTI_5 } ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA6()  { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_6 , EXTI5_9_IRQn  , GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_6 , EXTI_6 } ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA7()  { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_7 , EXTI5_9_IRQn  , GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_7 , EXTI_7 } ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA8()  { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_8 , EXTI5_9_IRQn  , GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_8 , EXTI_8 } ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA9()  { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_9 , EXTI5_9_IRQn  , GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_9 , EXTI_9 } ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA10() { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_10, EXTI10_15_IRQn, GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_10, EXTI_10} ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA11() { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_11, EXTI10_15_IRQn, GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_11, EXTI_11} ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA12() { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_12, EXTI10_15_IRQn, GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_12, EXTI_12} ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA13() { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_13, EXTI10_15_IRQn, GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_13, EXTI_13} ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA14() { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_14, EXTI10_15_IRQn, GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_14, EXTI_14} ; return *gpio ; }
    GpioIrq& GpioIrq::gpioA15() { static GpioIrq *gpio = new GpioIrq{RCU_GPIOA, GPIOA, GPIO_PIN_15, EXTI10_15_IRQn, GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_15, EXTI_15} ; return *gpio ; }
  }
}

extern "C"
{
  void EXTI0_IRQHandler()
  {
    if (exti_interrupt_flag_get(EXTI_0) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA0().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_0);
    }
  }

  void EXTI1_IRQHandler()
  {
    if (exti_interrupt_flag_get(EXTI_1) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA1().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_1);
    }
  }

  void EXTI2_IRQHandler()
  {
    if (exti_interrupt_flag_get(EXTI_2) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA2().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_2);
    }
  }

  void EXTI3_IRQHandler()
  {
    if (exti_interrupt_flag_get(EXTI_3) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA3().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_3);
    }
  }

  void EXTI4_IRQHandler()
  {
    if (exti_interrupt_flag_get(EXTI_4) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA4().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_4);
    }
  }

  void EXTI5_9_IRQHandler()
  {
    if (exti_interrupt_flag_get(EXTI_5) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA5().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_5);
    }
    if (exti_interrupt_flag_get(EXTI_6) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA6().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_6);
    }
    if (exti_interrupt_flag_get(EXTI_7) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA7().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_7);
    }
    if (exti_interrupt_flag_get(EXTI_8) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA8().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_8);
    }
    if (exti_interrupt_flag_get(EXTI_9) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA9().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_9);
    }
  }

  void EXTI10_15_IRQHandler()
  {
    if (exti_interrupt_flag_get(EXTI_10) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA10().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_10);
    }
    if (exti_interrupt_flag_get(EXTI_11) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA11().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_11);
    }
    if (exti_interrupt_flag_get(EXTI_12) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA12().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_12);
    }
    if (exti_interrupt_flag_get(EXTI_13) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA13().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_13);
    }
    if (exti_interrupt_flag_get(EXTI_14) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA14().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_14);
    }
    if (exti_interrupt_flag_get(EXTI_15) != RESET)
    {
      ::RV::GD32VF103::GpioIrq::gpioA15().irqHandler() ;
      exti_interrupt_flag_clear(EXTI_15);
    }
  }

}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
