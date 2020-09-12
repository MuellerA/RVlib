////////////////////////////////////////////////////////////////////////////////
// i2c.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace RV
{
  namespace GD32VF103
  {

    class I2c
    {
      I2c(uint32_t i2c, rcu_periph_enum rcuI2c, rcu_periph_enum rcuGpio, uint32_t gpio, uint32_t pinClk, uint32_t pinData) ;

    public:
      static I2c& i2c0() ;
      static I2c& i2c1() ;
  
      void setup(uint8_t address, uint32_t speed) ;
  
      bool put(uint8_t i2cAddress, uint8_t data) ;
      bool put(uint8_t i2cAddress, const uint8_t *data, size_t len) ;
      bool get(uint8_t i2cAddress, uint8_t &data) ;
      bool get(uint8_t i2cAddress, uint8_t *data, size_t len) ;

    private:
      bool waitFlag(uint32_t flag, bool set) ;
      bool waitFlagStopGen() ;
      
      uint32_t _i2c ;
      rcu_periph_enum _rcuI2c ;
      rcu_periph_enum _rcuGpio ;
      uint32_t _gpio ;
      uint32_t _pinClk ;
      uint32_t _pinData ;
    } ;

  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
    
