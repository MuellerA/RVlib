////////////////////////////////////////////////////////////////////////////////
// sdcard.h
////////////////////////////////////////////////////////////////////////////////
//
// uses SPI1[B13, B14, B15], B12
//
////////////////////////////////////////////////////////////////////////////////

#include "GD32VF103/gpio.h"
#include "GD32VF103/spi.h"
#include "GD32VF103/time.h"

#include <functional>

namespace RV
{
  namespace Longan
  {
    namespace FF
    {
      extern "C"
      {
#include "lib/fatfs-14/ff.h"
      }
    }

    class FatFs
    {
    public:
      using FileInfo = FF::FILINFO ;
      using Result = FF::FRESULT ;

      class File
      {
      public:
        Result open(const char *path, uint8_t mode) ;
        Result close() ;
        Result read(void *buff, size_t &len) ;
        Result write(const void *buff, size_t &len) ;
        Result seek (size_t ofs);
        Result truncate();
        Result sync();
        Result rewind() ;

      private:
        FF::FIL _file ;
      } ;

      class Dir
      {
      public:
        Result open(const char* path);
        Result close();
        Result read(FileInfo &fno);
        Result findfirst(FileInfo &fno, const char* path, const char* pattern);
        Result findnext(FileInfo &fno);
        Result rewind() ;
        
      private:
        FF::DIR _dir ;
      } ;
      
    private:
      FatFs() ;
      FatFs(const FatFs&) = delete ;
      
    public:
      struct Time
      {
        uint8_t year ;
        uint8_t month ;
        uint8_t day ;
        uint8_t hour ;
        uint8_t minute ;
        uint8_t second ;
      } ;
      using GetTime = std::function<void(Time&)> ;

      static FatFs& fatfs() ;

      void setup(const GetTime &getTime = nullptr) ;
      
      Result mkdir (const char* path);
      Result unlink (const char* path);
      Result rename (const char* path_old, const char* path_new);
      Result stat (const char* path, FileInfo &fno);
      //Result chmod (const char* path, uint8_t attr, uint8_t mask);
      //Result utime (const char* path, const FileInfo &fno);
      Result chdir (const char* path);
      Result chdrive (const char* path);
      //Result getcwd (char* buff, UINT len);
      Result getfree (const char* path, FF::DWORD* nclst, FF::FATFS** fatfs);
      //Result getlabel (const char* path, char *label, uint32_t *vsn);
      //Result setlabel (const char* label);
      //Result forward (File &file, UINT(*func)(const uint8_t*,UINT), UINT btf, UINT* bf);
      //Result expand (File &file, FSIZE_t fsz, uint8_t opt);
      //Result mount (FF::FATFS* fs, const char* path, uint8_t opt);
      Result mount ();
      Result unmount ();
      Result mkfs (const char* path, const FF::MKFS_PARM* opt, void* work, FF::UINT len);
      //Result fdisk (uint8_t pdrv, const LBA_t ptbl[], void* work);
      //Result setcp (WORD cp);
 
      //int putc (char c, File &file);
      //int puts (const char* str, FIL* cp);
      //int printf (File &file, const char* str, ...);
      //char* gets (char* buff, int len, File &file);

      /*
        #define eof(fp) ((int)((fp)->fptr == (fp)->obj.objsize))
        #define error(fp) ((fp)->err)
        #define tell(fp) ((fp)->fptr)
        #define size(fp) ((fp)->obj.objsize)
        #define rewind(fp) lseek((fp), 0)
        #define rewinddir(dp) readdir((dp), 0)
        #define rmdir(path) unlink(path)
        #define unmount(path) mount(0, path, 0)
      */
      
    private:
      friend FF::DWORD get_fattime (void) ;
      uint32_t fatTime() ;
      
      ::RV::GD32VF103::Spi &_spi ;
      ::RV::GD32VF103::Gpio &_cs ;

      FF::FATFS _fatfs ;
      GetTime _getTime ;
      Time _time ;
    } ;
  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
