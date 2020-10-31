////////////////////////////////////////////////////////////////////////////////
// lcd.cpp
////////////////////////////////////////////////////////////////////////////////

extern "C"
{
#include "gd32vf103.h"
}

#include "fatfs.h"

using ::RV::GD32VF103::TickTimer ;
using ::RV::GD32VF103::Spi ;
using ::RV::GD32VF103::Gpio ;

namespace RV
{
  namespace Longan
  {
    FatFs::FatFs()
      : _spi{Spi::spi1()}, _cs{Gpio::gpioB12()}, _time{20, 1, 1, 0, 0, 0}
    {
    }

    FatFs& FatFs::fatfs()
    {
      static FatFs *ff = new FatFs() ;
      return *ff ;
    }
    
    void FatFs::setup(const GetTime &getTime)
    {
      _spi.setup(Spi::Psc::_2) ;
      _cs.setup(Gpio::Mode::OUT_PP) ; _cs.high() ;
      _getTime = getTime ;
      
      mount() ;      
    }

    uint32_t FatFs::fatTime()
    {
      if (_getTime)
        _getTime(_time) ;
      return
        ((_time.year + 20) << 25) | // year - 1980
        ((_time.month    ) << 21) |
        ((_time.day      ) << 15) |
        ((_time.hour     ) << 11) |
        ((_time.minute   ) <<  5) |
        ((_time.second/2 ) <<  0) ; // second/2
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    
    FatFs::Result FatFs::File::open(const char *path, uint8_t mode)
    {
      return FF::f_open(&_file, path, mode) ;
    }
  
    FatFs::Result FatFs::File::close()
    {
      return FF::f_close(&_file) ;
    }
    
    FatFs::Result FatFs::File::read(void *buff, size_t &len)
    {
      return FF::f_read(&_file, buff, len, &len) ;
    }
    
    FatFs::Result FatFs::File::write(const void *buff, size_t &len)
    {
      return FF::f_write(&_file, buff, len, &len) ;
    }
    
    FatFs::Result FatFs::File::seek(size_t ofs)
    {
      return FF::f_lseek(&_file, ofs) ;
    }
    
    FatFs::Result FatFs::File::truncate()
    {
      return FF::f_truncate(&_file) ;
    }
    
    FatFs::Result FatFs::File::sync ()
    {
      return FF::f_sync(&_file) ;
    }

    FatFs::Result FatFs::File::rewind()
    {
      return FF::f_lseek(&_file, 0) ;
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    
    FatFs::Result FatFs::Dir::open(const char* path)
    {
      return FF::f_opendir(&_dir, path) ;
    }
    
    FatFs::Result FatFs::Dir::close()
    {
      return FF::f_closedir(&_dir) ;
    }
    
    FatFs::Result FatFs::Dir::read(FatFs::FileInfo &fno)
    {
      return FF::f_readdir(&_dir, &fno) ;
    }
    
    FatFs::Result FatFs::Dir::findfirst(FatFs::FileInfo &fno, const char* path, const char* pattern)
    {
      return FF::f_findfirst(&_dir, &fno, path, pattern) ;
    }
    
    FatFs::Result FatFs::Dir::findnext(FatFs::FileInfo &fno)
    {
      return FF::f_findnext(&_dir, &fno) ;
    }

    FatFs::Result FatFs::Dir::rewind()
    {
      return FF::f_readdir(&_dir, 0) ;
    }

    ////////////////////////////////////////////////////////////////////////////////
    
    FatFs::Result FatFs::mkdir (const char* path)
    {
      return FF::f_mkdir(path) ;
    }
    
    FatFs::Result FatFs::unlink (const char* path)
    {
      return FF::f_unlink(path) ;
    }
    
    FatFs::Result FatFs::rename (const char* path_old, const char* path_new)
    {
      return FF::f_rename(path_old, path_new) ;
    }
    
    FatFs::Result FatFs::stat (const char* path, FatFs::FileInfo &fno)
    {
      return FF::f_stat(path, &fno) ;
    }
    
    FatFs::Result FatFs::chdir(const char* path)
    {
      return FF::f_chdir(path) ;
    }
    
    FatFs::Result FatFs::chdrive (const char* path)
    {
      return FF::f_chdrive(path) ;
    }
    
    FatFs::Result FatFs::getfree (const char* path, FF::DWORD* nclst, FF::FATFS** fatfs)
    {
      return FF::f_getfree(path, nclst, fatfs) ;
    }
    
    FatFs::Result FatFs::mount ()
    {
      return FF::f_mount(&_fatfs, "", 0) ;
    }
    
    FatFs::Result FatFs::unmount ()
    {
      return FF::f_mount(nullptr, "", 0) ;
    }
    
    FatFs::Result FatFs::mkfs (const char* path, const FF::MKFS_PARM* opt, void* work, FF::UINT len)
    {
      return FF::f_mkfs(path, opt, work, len) ;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
