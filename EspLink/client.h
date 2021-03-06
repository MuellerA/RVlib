////////////////////////////////////////////////////////////////////////////////
// client.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>
#include <map>

#include "GD32VF103/usart.h"
#include "GD32VF103/time.h"

namespace RV
{
  namespace EspLink
  {
    enum class Cmd
    {
      Null = 0,         // null, mainly to prevent 0 from doing something bad 
      Sync,             // Synchronize, starts the protocol 
      RespV,            // Response with a value 
      RespCb,           // Response with a callback 
      WifiStatus,       // Get the wifi status 
      CbAdd,            // Add a custom callback 
      CbEvents,         // ??? 
      GetTime,          // Get current time in seconds since the unix epoch
      GetWifiInfo,      // Get ip address info
      //SetWifiInfo,    // Set ip address info
  
      MqttSetup = 10,   // Register callback functions 
      MqttPublish,      // Publish MQTT topic 
      MqttSubscribe,    // Subscribe to MQTT topic 
      MqttLwt,          // Define MQTT last will
      MqttGetClientId,  // 
  
      RestSetup = 20,   // Setup REST connection 
      RestRequest,      // Make request to REST server 
      RestSetHeader,    // Define HTML header 
  
      WebSetup = 30,    // web-server setup 
      WebData,          // used for publishing web-server data 
  
      SocketSetup = 40, // Setup socket connection 
      SocketSend,       // Send socket packet

      GetApCount = 50,  // Query the number of networks / Access Points known
      GetApName,        // Query the name (SSID) of an Access Point (AP)
      SelectSsid,       // Connect to a specific network
      SignalStrength,   // Query RSSI
      GetSsid,          // Query SSID currently connected to
      StartScan,        // Trigger a scan (takes a long time)
    } ;

    enum class WifiStatus
    {
      Idle = 0,         // Idle status 
      Connecting,       // Trying to connect 
      WrongPassword,    // Connection error, wrong password 
      NoApFound,        // Connection error, AP not found 
      ConnectFail,      // Connection error, connection failed 
      GotIp             // Connected, received IP 
    };

    struct Pdu
    {
      struct Param
      {
        bool val(uint8_t &v) const ;
        bool val(uint16_t &v) const ;
        bool val(uint32_t &v) const ;
        bool val(std::string &v) const ;
        bool val(uint8_t size, std::string &v) const ;
        bool val(uint8_t minSize, uint8_t maxSize, std::string &v) const ;
        
        uint8_t *_data ;
        uint32_t _len ;
      } ;
    
      static const uint32_t _size = 2000 ;
      uint8_t _raw[_size] ;
      uint32_t _len ;

      uint16_t _cmd ;
      uint16_t _argc ;
      uint32_t _ctx ;
      uint16_t _crc ;
      Param    _param[10] ;
    } ;

    class ClientCallback
    {
    public:
      virtual void rx(const Pdu&) = 0 ;
      virtual void close() = 0 ;
    } ;
  
    class Client
    {
      friend class WebServer ;
      friend class Socket ;
    public:
      Client(::RV::GD32VF103::Usart &usart) ;
      Client() = delete ;
      Client(const Client&) = delete ;
      Client& operator=(const Client&) = delete ;

      bool sync() ;
      bool webSetup(ClientCallback *cb) ;
      bool sockSetup(ClientCallback *cb, const std::string &host, uint16_t port, uint8_t mode, uint32_t &sockHdl) ;
      void wifiStatus(uint8_t &status) ;
      bool unixTime(uint32_t &time) ;
      bool wifiInfo(uint8_t *addr, uint8_t *mask, uint8_t *gw, uint8_t *mac) ;
      bool poll() ;
      const Pdu& rxPdu() ;
    
    private:
      friend class WifiCallback ;
      class WifiCallback : public ClientCallback
      {
      public:
        WifiCallback(Client &client) : _client{client}
        {
        }
        virtual void rx(const Pdu &rxPdu)
        {
          rxPdu._param[0].val(_client._wifiStatus) ;
        }
        virtual void close()
        {
        }
      private:
        Client &_client ;
      } ;
    
      static void crc(uint16_t &c, uint8_t b) ;
      void putNoEsc(uint8_t b) ;
      void put(uint8_t b) ;
      void put(const uint8_t *data, uint32_t len) ;
      bool get(uint8_t &b, bool &end) ;

      void send(Cmd cmd, uint32_t ctx, uint16_t argc) ; // start
      void send(uint8_t *data, uint16_t len) ;          // parameter * start.argc
      void send() ;                                     // end

      ::RV::GD32VF103::Usart &_usart ;
      uint16_t _crc ;
      Pdu _rxPdu ;

      WifiCallback _wifiCallback ;
      uint8_t  _wifiStatus{0} ;
      uint32_t _unixTime{0} ;
      ::RV::GD32VF103::TickTimer _unixTimeTick{3600000, true} ;
      ClientCallback *_callback[32] ; // [0]: nullptr, [1]: this->_wifiCallback, other: user's
    } ;
  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
