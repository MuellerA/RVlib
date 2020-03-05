////////////////////////////////////////////////////////////////////////////////
// time.cpp
////////////////////////////////////////////////////////////////////////////////

extern "C"
{
#include "gd32vf103.h"
}

#include "time.h"

namespace RV
{
  namespace GD32VF103
  {

    TickTimer::TickTimer(uint32_t ms, bool cyclic, bool exact)
      : _timeTick{now()}, _deltaTick{msToTick(ms)}, _cyclic{cyclic}, _exact{exact}
    {
    }

    TickTimer::TickTimer(uint32_t ms, uint32_t firstMs, bool cyclic, bool exact)
      : _timeTick{now() - msToTick(firstMs)+msToTick(ms)}, _deltaTick{msToTick(ms)}, _cyclic{cyclic}, _exact{exact}
    {
    }

    bool TickTimer::operator()()
    {
      uint64_t t = now() ;
      if ((t - _timeTick) < _deltaTick)
        return false ;
      if (_cyclic)
      {
        if (_exact)
          _timeTick += _deltaTick ;
        else
          _timeTick = t ;
      }
      return true ;
    }

    uint32_t TickTimer::elapsedMs() const
    {
      return tickToMs(now() - _timeTick) ;
    }
  
    void TickTimer::restart()
    {
      _timeTick = now() ;
    }

    uint64_t TickTimer::now()
    {
      return get_timer_value() ;
    }

    void TickTimer::delayMs(uint32_t ms)
    {
      uint64_t t0 = now() ;
      uint64_t d = msToTick(ms) ;

      while ((now() - t0) < d) ;
    }

    void TickTimer::delayUs(uint32_t us)
    {
      uint64_t t0 = now() ;
      uint64_t d = usToTick(us) ;

      while ((now() - t0) < d) ;
    }

    uint64_t TickTimer::msToTick(uint32_t ms)   { return (uint64_t)(SystemCoreClock / 4) * ms / 1000    ; }
    uint64_t TickTimer::usToTick(uint32_t us)   { return (uint64_t)(SystemCoreClock / 4) * us / 1000000 ; }
    uint32_t TickTimer::tickToMs(uint64_t tick) { return tick * 1000    / (SystemCoreClock / 4) ; }
    uint32_t TickTimer::tickToUs(uint64_t tick) { return tick * 1000000 / (SystemCoreClock / 4) ; }
  }  
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
