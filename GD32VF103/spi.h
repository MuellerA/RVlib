////////////////////////////////////////////////////////////////////////////////
// spi.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "io.h"

namespace RV
{
  namespace GD32VF103
  {

    class Spi : public ::RV::IO
    {
    private:
      Spi(uint32_t spi, rcu_periph_enum rcuSpi, rcu_periph_enum rcuGpio, uint32_t gpio, uint32_t pinClk, uint32_t pinMiso, uint32_t pinMosi) ;
      Spi(const Spi&) = delete ;
      
    public:
      void setup(uint32_t spiPsc = SPI_PSC_4) ;

      // RV::IO
      virtual bool get(uint8_t &b) ;
      virtual bool put(uint8_t  b) ;
      
      bool isTransmit() ;

      static Spi& spi0() ;
      static Spi& spi1() ;
      
    private:
      uint32_t _spi ;
      rcu_periph_enum _rcuSpi ;
      rcu_periph_enum _rcuGpio ;
      uint32_t _gpio ;
      uint32_t _pinClk ;
      uint32_t _pinMiso ;
      uint32_t _pinMosi ;
    } ;

  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
