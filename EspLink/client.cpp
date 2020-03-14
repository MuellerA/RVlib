////////////////////////////////////////////////////////////////////////////////
// client.cpp
////////////////////////////////////////////////////////////////////////////////

extern "C"
{
#include "gd32vf103.h"
}

#include <algorithm>

#include "GD32VF103/time.h"
#include "client.h"

using ::RV::GD32VF103::TickTimer ;
using ::RV::GD32VF103::Usart ;

namespace RV
{
  namespace EspLink
  {
    static const uint8_t SlipEnd    = 0b11000000U ;
    static const uint8_t SlipEsc    = 0b11011011U ;
    static const uint8_t SlipEscEnd = 0b11011100U ;
    static const uint8_t SlipEscEsc = 0b11011101U ;

    Client::Client(Usart &usart)
      : _usart{usart}, _wifiCallback{*this}
    {
      for (size_t i = 0 ; i < 32 ; ++i)
        _callback[i] = nullptr ;
      _callback[1] = &_wifiCallback ;
    }

    void Client::putNoEsc(uint8_t b)
    {
      while (!_usart.put(b))
        TickTimer::delayMs() ;
    }

    void Client::crc(uint16_t &c, uint8_t b)
    {
      c ^= b ;
      c  = (c >> 8) | (c << 8);
      c ^= (c & 0xff00) << 4;
      c ^= (c >> 8) >> 4;
      c ^= (c & 0xff00) >> 5;
    }
  
    void Client::put(uint8_t b)
    {
      crc(_crc, b) ;
    
      switch (b)
      {
      case SlipEnd: putNoEsc(SlipEsc) ; putNoEsc(SlipEscEnd) ; break ;
      case SlipEsc: putNoEsc(SlipEsc) ; putNoEsc(SlipEscEsc) ; break ;
      default:      putNoEsc(b) ;                              break ;
      }
    }

    void Client::put(const uint8_t *data, uint32_t len)
    {
      for (uint32_t i = 0 ; i < len ; ++i)
        put(*(data++)) ;
    }

    bool Client::get(uint8_t &b, bool &end)
    {
      static bool isEsc = false ;
      end = false ;
      if (!_usart.get(b))
        return false ;
      if (isEsc)
      {
        isEsc = false ;
        switch (b)
        {
        case SlipEscEnd: b = SlipEnd ; break ;
        case SlipEscEsc: b = SlipEsc ; break ;
        default: /* todo error */ ; break ;
        }
      }
      else if (b == SlipEsc)
      {
        isEsc = true ;
        return false ;
      }
      else if (b == SlipEnd)
      {
        end = true ;
      }
      return true ;
    }

    void Client::send(Cmd cmd, uint32_t ctx, uint16_t argc)
    {
      _crc = 0 ;
      putNoEsc(SlipEnd) ;
      put((uint8_t*)&cmd, 2) ;
      put((uint8_t*)&argc, 2) ;
      put((uint8_t*)&ctx, 4) ;
    }
  
    void Client::send(uint8_t *data, uint16_t len)
    {
      uint8_t padding[4] = { 0x00, 0x00, 0x00, 0x00 } ;
      put((uint8_t*)&len, 2) ;
      put(data, len) ;
      len = (4-((len)%4))%4 ;
      put(padding, len) ;
    }
  
    void Client::send()
    {
      uint16_t crc = _crc ; // _crc gets modified in put()
      put((uint8_t*)&crc, 2) ;
      putNoEsc(SlipEnd) ;
    }
  
    bool Client::sync()
    {
      _usart.clear() ; // reset
      _usart.put(SlipEnd) ; // new start
    
      _rxPdu._len = 0 ; // prepare pdu

      send(Cmd::Sync, 1 /* -> _wifiCallback */, 0) ;
      send() ;

      return true ;
    }

    bool Client::webSetup(ClientCallback *cb)
    {
      uint32_t id ;
      for (id = 2 ; (id < 32) && (_callback[id] != cb) && _callback[id] ; ++id) ;
      if (id == 32)
        return false ;

      _callback[id] = cb ;
      send(Cmd::WebSetup, 0, 1) ;
      send((uint8_t*)&id, 4) ;
      send() ;
      return true ;
    }

    bool Client::sockSetup(ClientCallback *cb, const std::string &host, uint16_t port, uint8_t mode, uint32_t &sockHdl)
    {
      uint32_t id ;
      sockHdl = ~0 ;
    
      for (id = 2 ; (id < 32) && (_callback[id] != cb) && _callback[id] ; ++id) ;
      if (id == 32)
        return false ;

      _callback[id] = cb ;
    
      send(Cmd::SocketSetup, id, 3) ;
      send((uint8_t*)host.data(), host.size()) ;
      send((uint8_t*)&port, sizeof(port)) ;
      send((uint8_t*)&mode, sizeof(uint8_t)) ;
      send() ;

      TickTimer timeout{1200} ;
      while (!poll() || ((Cmd)_rxPdu._cmd != Cmd::RespV))
      {
        if (timeout())
          return false ;
      }
      sockHdl = _rxPdu._ctx ;
    
      return true ;
    }    
  
    void Client::wifiStatus(uint8_t &status)
    {
      //dont call esp, use cached value instead
      //send(Cmd::WifiStatus, 0, 0) ;
      //send() ;
      status = _wifiStatus ;
    }

    bool Client::unixTime(uint32_t &time)
    {
      // use local time if possible
      if ((_unixTime < 946681200) ||   // got no time yet
          _unixTimeTick())             // refresh
      {
        send(Cmd::GetTime, 0, 0) ;
        send() ;

        TickTimer timeout{1000} ;
        while (!poll() || ((Cmd)_rxPdu._cmd != Cmd::RespV))
        {
          if (timeout())
            return false ;
        }
        _unixTime = _rxPdu._ctx ;
      }
      time = _unixTime + _unixTimeTick.elapsedMs()/1000 ;
      return true ;
    }

    bool Client::wifiInfo(uint8_t *addr, uint8_t *mask, uint8_t *gw, uint8_t *mac)
    {
      if ((WifiStatus)_wifiStatus != WifiStatus::GotIp)
        return false ;
      send(Cmd::GetWifiInfo, 999, 0) ; // ctx != 0
      send() ;

      TickTimer timeout{1000} ;
      while (!poll() || ((Cmd)_rxPdu._cmd != Cmd::RespCb) || (_rxPdu._ctx != 999))
      {
        if (timeout())
          return false ;
      }

      std::string ipaddr ;
      std::string ipmask ;
      std::string ipgw ;
      std::string ipmac ;

      if ((_rxPdu._argc != 4) ||
          !_rxPdu._param[0].val(4, ipaddr) ||
          !_rxPdu._param[1].val(4, ipmask) ||
          !_rxPdu._param[2].val(4, ipgw  ) ||
          !_rxPdu._param[3].val(6, ipmac ))
        return false ;

      std::reverse(ipaddr.begin(), ipaddr.end()) ;
      std::reverse(ipmask.begin(), ipmask.end()) ;
      std::reverse(ipgw  .begin(), ipgw  .end()) ;

      ipaddr.copy((char*)addr, 4) ;
      ipmask.copy((char*)mask, 4) ;
      ipgw  .copy((char*)gw  , 4) ;
      ipmac .copy((char*)mac , 6) ;

      return true ;
    }

    bool Client::poll()
    {
      while (true)
      {
        uint8_t b ;
        bool end ;

        if (!get(b, end))
          return false ;

        if (end)
          break ;

        if (_rxPdu._len == _rxPdu._size)
        {
          _rxPdu._len = 0 ; // todo error
          return false ;
        }
        _rxPdu._raw[_rxPdu._len++] = b ;
      }

      if (_rxPdu._len == 0) // rx sync
        return false ;
    
      if (_rxPdu._len < 10)
      {
        _rxPdu._len = 0 ; // todo error
        return false ;
      }
    
      // decode header
      _rxPdu._cmd  = *(uint16_t*) (_rxPdu._raw + 0x00) ;
      _rxPdu._argc = *(uint16_t*) (_rxPdu._raw + 0x02) ;
      _rxPdu._ctx  = *(uint32_t*) (_rxPdu._raw + 0x04) ;

      // check parameters
      uint8_t *offset = _rxPdu._raw + 8 ;
      uint8_t *rawEnd = _rxPdu._raw + _rxPdu._len - 2 ;
      for (uint32_t iArg = 0, eArg = _rxPdu._argc ; iArg < eArg ; ++iArg)
      {
        _rxPdu._param[iArg]._len  = *(uint16_t*)offset ;
        offset += 2 ;
        _rxPdu._param[iArg]._data = offset ; 
        offset += _rxPdu._param[iArg]._len ;
        offset += (4-((_rxPdu._param[iArg]._len+2)%4))%4 ; // padding
        if (offset > rawEnd)
        {
          _rxPdu._len = 0 ; // todo error
          return false ;
        }
      }

      if (offset != rawEnd)
      {
        _rxPdu._len = 0 ; // todo error
        return false ;
      }
      
      // check crc
      _rxPdu._crc = 0 ;
      for (uint32_t i = 0, e = _rxPdu._len - 2 ; i < e ; ++i)
        crc(_rxPdu._crc, _rxPdu._raw[i]) ;
      if (_rxPdu._crc != *(uint16_t*)(_rxPdu._raw + _rxPdu._len - 2))
      {
        _rxPdu._len = 0 ; // todo error
        return false ;
      }

      switch ((EspLink::Cmd)_rxPdu._cmd)
      {
      case Cmd::Sync:
        break ;
      case Cmd::RespV:
        break ;
      case Cmd::RespCb:
        {
          uint32_t ctx = _rxPdu._ctx ;
          if ((ctx < 32) && _callback[ctx])
            _callback[ctx]->rx(_rxPdu) ;
          // todo error
          break ;
        }
      default:
        // todo error
        break ;
      }
      _rxPdu._len = 0 ;
    
      return true ;
    }

    const Pdu& Client::rxPdu()
    {
      return _rxPdu ;
    }

    bool Pdu::Param::val(uint8_t &v) const
    {
      if (_len != sizeof(uint8_t))
        return false ;
      v = *_data ;
      return true ;
    }
  
    bool Pdu::Param::val(uint16_t &v) const
    {
      if (_len != sizeof(uint16_t))
        return false ;
      v = *(uint16_t*)_data;
      return true ;
    }
  
    bool Pdu::Param::val(uint32_t &v) const
    {
      if (_len != sizeof(uint32_t))
        return false ;
      v = *(uint32_t*)_data;
      return true ;
    }
  
    bool Pdu::Param::val(std::string &v) const
    {
      v.assign((char*)_data, _len) ;
      return true ;
    }
  
    bool Pdu::Param::val(uint8_t size, std::string &v) const
    {
      if (_len != size)
        return false ;
      
      v.assign((char*)_data, _len) ;
      return true ;
    }
  
    bool Pdu::Param::val(uint8_t minSize, uint8_t maxSize, std::string &v) const
    {
      if ((_len < minSize) || (maxSize < _len))
        return false ;
      
      v.assign((char*)_data, _len) ;
      return true ;
    }
  
  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
