////////////////////////////////////////////////////////////////////////////////
// diskio.c
////////////////////////////////////////////////////////////////////////////////

// based on examples from http://elm-chan.org/fsw/ff/ffsample.zip

extern "C"
{
#include "gd32vf103.h"
}

#include "fatfs.h"

using ::RV::GD32VF103::TickTimer ;
using ::RV::GD32VF103::Spi ;
using ::RV::GD32VF103::Gpio ;

Spi   &spi{Spi::spi1()} ; // gpio b13,b14,b15
Gpio  &gpioCS{Gpio::gpioB12()} ;

////////////////////////////////////////////////////////////////////////////////

namespace RV
{
  namespace Longan
  {
    namespace FF
    {
      extern "C"
      {
#include "lib/fatfs-14/diskio.h"
      }
    }
    extern "C"
    {
      FF::DSTATUS disk_initialize (FF::BYTE pdrv);
      FF::DSTATUS disk_status (FF::BYTE pdrv);
      FF::DRESULT disk_read (FF::BYTE pdrv, FF::BYTE* buff, FF::LBA_t sector, FF::UINT count);
      FF::DRESULT disk_write (FF::BYTE pdrv, const FF::BYTE* buff, FF::LBA_t sector, FF::UINT count);
      FF::DRESULT disk_ioctl (FF::BYTE pdrv, FF::BYTE cmd, void* buff);
      FF::DWORD   get_fattime (void) ;
    }
    
    static FF::DSTATUS Stat = STA_NOINIT;       // Disk status
    static FF::BYTE    CardType;                // b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing

    ////////////////////////////////////////////////////////////////////////////////

    // MMC/SD command (SPI mode)
    static const uint8_t CMD0   = 0      ;       // GO_IDLE_STATE
    static const uint8_t CMD1   = 1      ;       // SEND_OP_COND
    static const uint8_t ACMD41 = 0x80+41;       // SEND_OP_COND (SDC)
    static const uint8_t CMD8   = 8      ;       // SEND_IF_COND
    static const uint8_t CMD9   = 9      ;       // SEND_CSD
    static const uint8_t CMD10  = 10     ;       // SEND_CID
    static const uint8_t CMD12  = 12     ;       // STOP_TRANSMISSION
    static const uint8_t CMD13  = 13     ;       // SEND_STATUS
    static const uint8_t ACMD13 = 0x80+13;       // SD_STATUS (SDC)
    static const uint8_t CMD16  = 16     ;       // SET_BLOCKLEN
    static const uint8_t CMD17  = 17     ;       // READ_SINGLE_BLOCK
    static const uint8_t CMD18  = 18     ;       // READ_MULTIPLE_BLOCK
    static const uint8_t CMD23  = 23     ;       // SET_BLOCK_COUNT
    static const uint8_t ACMD23 = 0x80+23;       // SET_WR_BLK_ERASE_COUNT (SDC)
    static const uint8_t CMD24  = 24     ;       // WRITE_BLOCK
    static const uint8_t CMD25  = 25     ;       // WRITE_MULTIPLE_BLOCK
    static const uint8_t CMD32  = 32     ;       // ERASE_ER_BLK_START
    static const uint8_t CMD33  = 33     ;       // ERASE_ER_BLK_END
    static const uint8_t CMD38  = 38     ;       // ERASE
    static const uint8_t CMD55  = 55     ;       // APP_CMD
    static const uint8_t CMD58  = 58     ;       // READ_OCR

    ////////////////////////////////////////////////////////////////////////////////

    static inline void tx(const FF::BYTE* buff, FF::UINT size)
    {
      spi.xch(const_cast<uint8_t*>(buff), size, 1) ;
    }

    static inline void rx(FF::BYTE *buff, FF::UINT size)
    {
      spi.xch(buff, size, 2) ;
    }

    static bool wait_ready(FF::UINT timeoutMs = 500)
    {
      FF::BYTE d;
      TickTimer timeout(timeoutMs) ;

      while (!timeout())
      {
        rx(&d, 1) ;
        if (d == 0xff)
          return true ;
        TickTimer::delayUs(100) ;
      }
      return false ;
    }

    static void deselect()
    {
      FF::BYTE d = 0xff ;

      gpioCS.high();
      tx(&d, 1) ;     // Dummy clock (force DO hi-z for multiple slave SPI)
    }

    static bool select()
    {
      FF::BYTE d = 0xff ;

      gpioCS.low();
      tx(&d, 1); // Dummy clock (force DO enabled)
      if (!wait_ready())
      {
        deselect() ;
        return false ;
      }

      return true ;
    }

    static bool rx_datablock(FF::BYTE *buff,  // Data buffer to store received data
                             FF::UINT btr)    // Byte count
    {
      FF::BYTE d[2];
      TickTimer timeout(100) ;

      while (true)
      {
        rx(d, 1);
        if (d[0] != 0xFF)
          break;
        if (timeout())
          return false ;
        TickTimer::delayUs(100);
      }
      if (d[0] != 0xFE)
        return false;           // If not valid data token, return with error

      rx(buff, btr);            // Receive the data block into buffer
      rx(d, 2);                 // Discard CRC

      return true;              // Return with success
    }

#if FF_FS_READONLY == 0
    static bool tx_datablock(const FF::BYTE *buff,  //512 byte data block to be transmitted
                             FF::BYTE token)        // Data/Stop token
    {
      FF::BYTE d[2];

      if (!wait_ready())
        return false;

      tx(&token, 1);            // tx a token
      if (token != 0xFD)        // Send data if token is other than StopTran
      {
        tx(buff, 512);          // Xmit the 512 byte data block to MMC
        rx(d, 2);               // Xmit dummy CRC (0xFF,0xFF)
        rx(d, 1);               // Receive data response
        if ((d[0] & 0x1F) != 0x05)  // If not accepted, return with error
          return false;
      }

      return true;
    }
#endif

    // Returns command response (bit7==1:Send failed)*/
    static FF::BYTE send_cmd (FF::BYTE cmd,         // Command byte
                              FF::DWORD arg)        // Argument
    {
      FF::BYTE n, d, buf[6];

      if (cmd & 0x80)       // ACMD<n> is the command sequense of CMD55-CMD<n>
      {
        cmd &= 0x7F;
        n = send_cmd(CMD55, 0);
        if (n > 1)
          return n;
      }

      // Select the card and wait for ready except to stop multiple block read
      if (cmd != CMD12)
      {
        deselect();
        if (!select())
          return 0xFF;
      }

      // Send a command packet
      buf[0] = 0x40 | cmd;                  // Start + Command index
      buf[1] = (FF::BYTE)(arg >> 24);           // Argument[31..24]
      buf[2] = (FF::BYTE)(arg >> 16);           // Argument[23..16]
      buf[3] = (FF::BYTE)(arg >> 8);            // Argument[15..8]
      buf[4] = (FF::BYTE)arg;                   // Argument[7..0]
      n = 0x01;                             // Dummy CRC + Stop
      if (cmd == CMD0) n = 0x95;            // (valid CRC for CMD0(0))
      if (cmd == CMD8) n = 0x87;            // (valid CRC for CMD8(0x1AA))
      buf[5] = n;
      tx(buf, 6);

      // Receive command response
      if (cmd == CMD12)
        rx(&d, 1);      // Skip a stuff byte when stop reading
      n = 10;               // Wait for a valid response in timeout of 10 attempts
      for (FF::BYTE i = 0 ; i < 10 ; ++i)
      {
        rx(&d, 1);
        if (!(d & 0x80))
          break ;
      }

      return d;   // Return with the response value
    }

    ////////////////////////////////////////////////////////////////////////////////

    FF::DSTATUS disk_initialize (FF::BYTE drv) // [IN] Physical drive number
    {
      FF::BYTE type, cmd, buf[10];

      if (drv)
        return FF::RES_NOTRDY;

      TickTimer::delayMs(10);

      rx(buf, 10);                        // Apply 80 dummy clocks and the card gets ready to receive command

      type = 0;
      if (send_cmd(CMD0, 0) == 1)         // Enter Idle state
      {
        if (send_cmd(CMD8, 0x1AA) == 1)   // SDv2?
        {
          rx(buf, 4);                     // Get trailing return value of R7 resp
          if (buf[2] == 0x01 && buf[3] == 0xAA)   // The card can work at vdd range of 2.7-3.6V
          {
            TickTimer timeout(1000) ;
            while (!timeout())
            {
              if (send_cmd(ACMD41, 1UL << 30) == 0)
              {
                if (send_cmd(CMD58, 0) == 0)   // Check CCS bit in the OCR
                {
                  rx(buf, 4);
                  type = (buf[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;  // SDv2
                }
                break ;
              }
              TickTimer::delayMs(1) ;
            }
          }
        }
        else              // SDv1 or MMCv3
        {
          if (send_cmd(ACMD41, 0) <= 1)
          {
            type = CT_SD1;
            cmd = ACMD41;  // SDv1
          }
          else
          {
            type = CT_MMC;
            cmd = CMD1;  // MMCv3
          }
          TickTimer timeout(1000) ;
          while (true)
          {
            if (send_cmd(cmd, 0) == 0)
            {
              if (send_cmd(CMD16, 512) != 0)
                type = 0 ;
              break;
            }
            if (timeout())
            {
              type = 0 ;
              break ;
            }
          }
        }
      }
      CardType = type;
      Stat = type ? 0 : STA_NOINIT;

      deselect();

      return Stat;
    }

    FF::DSTATUS disk_status (FF::BYTE drv)     // [IN] Physical drive number
    {
      if (drv)
        return STA_NOINIT ;

      return Stat ;
    }

    FF::DRESULT disk_read(FF::BYTE drv,      // [IN] Physical drive number
                          FF::BYTE* buff,    // [OUT] Pointer to the read data buffer
                          FF::LBA_t sector,  // [IN] Start sector number
                          FF::UINT count)    // [IN] Number of sectros to read
    {
      FF::DWORD sect = (FF::DWORD)sector;

      if (drv || !count)
        return FF::RES_PARERR ;

      if (Stat & STA_NOINIT)
        return FF::RES_NOTRDY;

      if (!(CardType & CT_BLOCK))      // Convert LBA to byte address if needed
        sect *= 512;

      FF::UINT iRx = 0 ;
      if (count == 1)
      {
        if ((send_cmd(CMD17, sect) == 0) &&
            rx_datablock(buff, 512))
          iRx = 1 ;
      }
      else
      {
        if (send_cmd(CMD18, sect) == 0)
        {
          for (iRx = 0 ; iRx < count ; ++iRx)
          {
            if (!rx_datablock(buff, 512))
              break;
            buff += 512;
          }
          send_cmd(CMD12, 0);       // STOP_TRANSMISSION
        }
      }

      deselect();

      return (iRx == count) ? FF::RES_OK : FF::RES_ERROR ;
    }

#if FF_FS_READONLY == 0
    FF::DRESULT disk_write (FF::BYTE drv,         // [IN] Physical drive number
                            const FF::BYTE* buff, // [IN] Pointer to the data to be written
                            FF::LBA_t sector,     // [IN] Sector number to write from
                            FF::UINT count)       // [IN] Number of sectors to write
    {
      FF::DWORD sect = (FF::DWORD)sector;

      if (drv || !count)
        return FF::RES_PARERR ;
      if (Stat & STA_NOINIT)
        return FF::RES_NOTRDY;
      if (!(CardType & CT_BLOCK))
        sect *= 512;                        // Convert LBA to byte address if needed

      FF::UINT iTx = 0 ;
      if (count == 1)                       // Single block write
      {
        if ((send_cmd(CMD24, sect) == 0) && // WRITE_BLOCK
            tx_datablock(buff, 0xFE))
          iTx = 1;
      }
      else                                  // Multiple block write
      {
        if (CardType & CT_SDC)
          send_cmd(ACMD23, count);
        if (send_cmd(CMD25, sect) == 0)     // WRITE_MULTIPLE_BLOCK
        {
          for (iTx = 0 ; iTx < count ; ++iTx)
          {
            if (!tx_datablock(buff, 0xFC))
              break;
            buff += 512;
          }
          if (!tx_datablock(0, 0xFD))       // STOP_TRAN token
            iTx = 0 ;
        }
      }
      deselect();

      return (count == iTx) ? FF::RES_OK : FF::RES_ERROR;
    }
#endif

    FF::DRESULT disk_ioctl (FF::BYTE drv,       // [IN] Drive number
                            FF::BYTE ctrl,      // [IN] Control command code
                            void* buff)     // [I/O] Parameter and data buffer
    {
      FF::DRESULT res;
      FF::BYTE n, csd[16];
      FF::DWORD cs;
      FF::BYTE dummy ;

      if (drv)
        return FF::RES_PARERR ;
      if (Stat & STA_NOINIT)
        return FF::RES_NOTRDY;          // Check if card is in the socket

      res = FF::RES_ERROR;
      switch (ctrl)
      {
      case CTRL_SYNC :              // Make sure that no pending write process
        if (select())
          res = FF::RES_OK;
        break;

      case GET_SECTOR_COUNT :       // Get number of sectors on the disk (FF::DWORD)
        if ((send_cmd(CMD9, 0) == 0) && rx_datablock(csd, 16))
        {
          if ((csd[0] >> 6) == 1)   // SDC ver 2.00
          {
            cs = csd[9] + ((FF::WORD)csd[8] << 8) + ((FF::DWORD)(csd[7] & 63) << 16) + 1;
            *(FF::LBA_t*)buff = cs << 10;
          }
          else                                      // SDC ver 1.XX or MMC
          {
            n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
            cs = (csd[8] >> 6) + ((FF::WORD)csd[7] << 2) + ((FF::WORD)(csd[6] & 3) << 10) + 1;
            *(FF::LBA_t*)buff = cs << (n - 9);
          }
          res = FF::RES_OK;
        }
        break;

      case GET_BLOCK_SIZE :    // Get erase block size in unit of sector (FF::DWORD)
        if (CardType & CT_SD2) // SDC ver 2.00
        {
          if (send_cmd(ACMD13, 0) == 0) // Read SD status
          {
            rx(&dummy, 1);
            if (rx_datablock(csd, 16)) // Read partial block
            {
              for (n = 64 - 16; n; n--)
                rx(&dummy, 1);  // Purge trailing data
              *(FF::DWORD*)buff = 16UL << (csd[10] >> 4);
              res = FF::RES_OK;
            }
          }
        }
        else // SDC ver 1.XX or MMC
        {
          if ((send_cmd(CMD9, 0) == 0) && rx_datablock(csd, 16)) // Read CSD
          {
            if (CardType & CT_SD1) // SDC ver 1.XX
            {
              *(FF::DWORD*)buff = (((csd[10] & 63) << 1) + ((FF::WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
            }
            else // MMC
            {
              *(FF::DWORD*)buff = ((FF::WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
            }
            res = FF::RES_OK;
          }
        }

        break;

      case CTRL_TRIM :            // Erase a block of sectors (used when _USE_ERASE == 1)
        {
          FF::LBA_t *dp;
          FF::DWORD st, ed ;

          if (!(CardType & CT_SDC)) // Check if the card is SDC
            break;
          if (disk_ioctl(drv, MMC_GET_CSD, csd))   // Get CSD
            break;
          if (!(csd[0] >> 6) && !(csd[10] & 0x40)) // Check if sector erase can be applied to the card
            break;

          dp = (FF::LBA_t*)buff;
          st = (FF::DWORD)dp[0];
          ed = (FF::DWORD)dp[1];                     // Load sector block
          if (!(CardType & CT_BLOCK))
          {
            st *= 512;
            ed *= 512;
          }

          if (send_cmd(CMD32, st) == 0 &&  // Erase sector block
              send_cmd(CMD33, ed) == 0 &&
              send_cmd(CMD38, 0) == 0 &&
              wait_ready(30000))
          {
            res = FF::RES_OK;   // FatFs does not check result of this command */
          }
        }
        break;

      case MMC_GET_CSD:             // Receive CSD as a data block (16 bytes)
        if (send_cmd(CMD9, 0) == 0 && rx_datablock((FF::BYTE*)buff, 16))       // READ_CSD
          res = FF::RES_OK;
        deselect();
        break;

      default:
        res = FF::RES_PARERR;
      }

      deselect();

      return res;
    }

    FF::DWORD get_fattime (void)
    {
      return FatFs::fatfs().fatTime() ;
    }

  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
