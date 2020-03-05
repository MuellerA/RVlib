////////////////////////////////////////////////////////////////////////////////
// time.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace RV
{
  namespace GD32VF103
  {
    class TickTimer
    {
      // sys tick is SytemCoreClock/4 Hz
    public:
      TickTimer(uint32_t ms, bool cyclic = false, bool exact = false) ;
      TickTimer(uint32_t ms, uint32_t firstMs, bool cyclic = false, bool exact = false) ;
      TickTimer(const TickTimer&) = delete ;
      
      bool operator()() ; // has expired (and restart if cyclic==true)
      uint32_t elapsedMs() const ; // ms since started
      void restart() ;

      static uint64_t now() ;
      static void delayMs(uint32_t ms = 1) ; // active wait
      static void delayUs(uint32_t us = 1) ;
      static uint64_t msToTick(uint32_t ms)   ;
      static uint64_t usToTick(uint32_t us)   ;
      static uint32_t tickToMs(uint64_t tick) ;
      static uint32_t tickToUs(uint64_t tick) ;

    private:
      uint64_t _timeTick ;
      uint64_t _deltaTick ;
      bool _cyclic ;
      bool _exact ;
    } ;
      
  }    
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
