////////////////////////////////////////////////////////////////////////////////
// usart.cpp
////////////////////////////////////////////////////////////////////////////////

extern "C"
{
#include "gd32vf103.h"
}

#include "malloc.h"
#include "usart.h"

namespace RV
{
  namespace GD32VF103
  {

    ////////////////////////////////////////////////////////////////////////////////

    Fifo::Fifo(size_t size) : _size{size}, _mask{size-1},_in{0}, _out{0}
    {
      _data = (uint8_t*)malloc(size) ;
    }
    Fifo::~Fifo()
    {
      free((void*)_data) ;
    }

    bool Fifo::empty() const
    {
      return _in == _out ;
    }

    bool Fifo::full() const
    {
      return ((_in+1) & _mask) == _out ;
    }

    bool Fifo::get(uint8_t &b)
    {
      if (empty())
        return false ;
      b = _data[_out++] ;
      _out &= _mask ;
      return true ;
    }
    bool Fifo::put(uint8_t b)
    {
      if (full())
        return false ;
      _data[_in++] = b ;
      _in &= _mask ;
      return true ;
    }

    void Fifo::clear()
    {
      _out = _in ;
    }
    
    ////////////////////////////////////////////////////////////////////////////////

    Usart::Usart(uint32_t usart, rcu_periph_enum rcuGpio, rcu_periph_enum rcuUsart, uint32_t irq, uint32_t gpio, uint32_t txPin, uint32_t rxPin)
      : _usart{usart}, _rcuGpio{rcuGpio}, _rcuUsart{rcuUsart}, _irq{irq}, _gpio{gpio}, _txPin{txPin}, _rxPin{rxPin}, _txData{64}, _rxData{256}
    {
    }

    void Usart::setup(uint32_t baud)
    {
      rcu_periph_clock_enable(_rcuGpio);  // enable GPIO clock
      rcu_periph_clock_enable(_rcuUsart); // enable USART clock

      gpio_init(_gpio, GPIO_MODE_AF_PP,       GPIO_OSPEED_50MHZ, _txPin);
      gpio_init(_gpio, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, _rxPin);

      // USART 115200,8N1, no-flow-control
      usart_deinit(_usart);
      usart_baudrate_set(_usart, baud);
      usart_word_length_set(_usart, USART_WL_8BIT);
      usart_stop_bit_set(_usart, USART_STB_1BIT);
      usart_parity_config(_usart, USART_PM_NONE);
      usart_hardware_flow_rts_config(_usart, USART_RTS_DISABLE);
      usart_hardware_flow_cts_config(_usart, USART_CTS_DISABLE);
      usart_receive_config(_usart, USART_RECEIVE_ENABLE);
      usart_transmit_config(_usart, USART_TRANSMIT_ENABLE);
      usart_enable(_usart);

      eclic_global_interrupt_enable();
      eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
      eclic_irq_enable(_irq, 1, 0);

      usart_interrupt_enable(_usart, USART_INT_TBE);
      usart_interrupt_enable(_usart, USART_INT_RBNE);
    }

    bool Usart::get(uint8_t &b)
    {
      usart_interrupt_disable(_usart, USART_INT_RBNE);
      bool res = _rxData.get(b) ;
      usart_interrupt_enable(_usart, USART_INT_RBNE);
      return res ;
    }  

    bool Usart::put(uint8_t b)
    {
      usart_interrupt_disable(_usart, USART_INT_TBE);
      bool res = _txData.put(b) ;
      usart_interrupt_enable(_usart, USART_INT_TBE);
      return res ;
    }

    void Usart::clear()
    {
      _rxData.clear() ;
      _txData.clear() ;
    }

    void Usart::baud(uint32_t baud)
    {
      usart_baudrate_set(_usart, baud) ;
    }
    
    Usart& Usart::usart0()
    {
      static Usart *usart0 = new Usart{USART0, RCU_GPIOA, RCU_USART0, USART0_IRQn, GPIOA, GPIO_PIN_9, GPIO_PIN_10} ;
      
      return *usart0 ;
    }
    
    Usart& Usart::usart1()
    {
      static Usart *usart1 = new Usart{USART1, RCU_GPIOA, RCU_USART1, USART1_IRQn, GPIOA, GPIO_PIN_2, GPIO_PIN_3} ;
      
      return *usart1 ;
    }
    
    Usart& Usart::usart2()
    {
      static Usart *usart2 = new Usart{USART2, RCU_GPIOB, RCU_USART2, USART2_IRQn, GPIOB, GPIO_PIN_10, GPIO_PIN_11} ;
      
      return *usart2 ;
    }

    void Usart::irqHandler()
    {
      if (usart_interrupt_flag_get(_usart, USART_INT_FLAG_RBNE) != RESET) // RX
      {
        usart_interrupt_disable(_usart, USART_INT_RBNE);
        uint8_t b = usart_data_receive(_usart) ;
        _rxData.put(b) ;
        usart_interrupt_enable(_usart, USART_INT_RBNE);
      }
      if (usart_interrupt_flag_get(_usart, USART_INT_FLAG_TBE) != RESET)
      {
        usart_interrupt_disable(_usart, USART_INT_TBE);    
        uint8_t b ;

        if (_txData.get(b))
        {
          usart_data_transmit(_usart, b) ;
          usart_interrupt_enable(_usart, USART_INT_TBE);
        }
      }
    }

  }
}

extern "C"
{
  void USART0_IRQHandler() { RV::GD32VF103::Usart::usart0().irqHandler() ; }
  void USART1_IRQHandler() { RV::GD32VF103::Usart::usart1().irqHandler() ; }
  void USART2_IRQHandler() { RV::GD32VF103::Usart::usart2().irqHandler() ; }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
