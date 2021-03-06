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
      enum class Psc
        {
         _2   = SPI_PSC_2,
         _4   = SPI_PSC_4,
         _8   = SPI_PSC_8,
         _16  = SPI_PSC_16,
         _32  = SPI_PSC_32,
         _64  = SPI_PSC_64,
         _128 = SPI_PSC_128,
         _256 = SPI_PSC_256,
         
        } ;
      void setup(Psc spiPsc = Psc::_4) ;

      // RV::IO
      virtual bool get(uint8_t &b) ;
      virtual bool put(uint8_t  b) ;

      bool xch(uint8_t &b) ;
      bool xch(uint8_t *data, size_t size, uint8_t mode) ; // mode 1:tx, 2:rx, 3:txrx
      
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
