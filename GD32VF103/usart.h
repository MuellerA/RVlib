////////////////////////////////////////////////////////////////////////////////
// usart.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "io.h"

extern "C"
{
  void USART0_IRQHandler() ;
  void USART1_IRQHandler() ;
  void USART2_IRQHandler() ;
}
    
namespace RV
{
  namespace GD32VF103
  {
    
    class Fifo
    {
    public:
      Fifo(size_t size) ;
      ~Fifo() ;
      bool empty() const ;
      bool full() const ;
      bool get(uint8_t &b) ;
      bool put(uint8_t  b) ;
      void clear() ;

    private:
      const uint32_t _size ; // must be 2^n
      const uint32_t _mask ; // _size - 1 ;

      volatile uint32_t _in ; 
      volatile uint32_t _out ;
      volatile uint8_t  *_data ;
    } ;

    class Usart : public ::RV::IO
    {
    private:
      friend void ::USART0_IRQHandler() ;
      friend void ::USART1_IRQHandler() ;
      friend void ::USART2_IRQHandler() ;
      
      Usart(uint32_t usart, rcu_periph_enum rcuGpio, rcu_periph_enum rcuUsart, uint32_t irq, uint32_t gpio, uint32_t txPin, uint32_t rxPin) ;
      Usart(const Usart&) = delete ;
      
    public:
      void setup(uint32_t baud = 115200) ;

      // RV::IO
      virtual bool get(uint8_t &b) ;
      virtual bool put(uint8_t  b) ;

      void clear() ;
      void baud(uint32_t baud) ;
      
      static Usart& usart0() ;
      static Usart& usart1() ;
      static Usart& usart2() ;
      
    private:
      void irqHandler() ;
      
      const uint32_t        _usart ;
      const rcu_periph_enum _rcuGpio ;
      const rcu_periph_enum _rcuUsart ;
      const uint32_t        _irq ;
      const uint32_t        _gpio ;
      const uint32_t        _txPin ;
      const uint32_t        _rxPin ;

      Fifo _txData ;
      Fifo _rxData ;
    } ;

  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
