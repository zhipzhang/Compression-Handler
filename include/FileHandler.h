#pragma once
#include <cstddef>
class FileHandler {
   public:
    virtual ~FileHandler() {}
    //------------------------------------------------------------------------
    //! Similar to the C-file read method
    //!
    //! @param buffer          buffer to put data in
    //! @param size            number of bytes want to read
    //! @return                number of bytes actually read
    //------------------------------------------------------------------------
    virtual size_t read(unsigned char* buffer, size_t size) = 0;
    //------------------------------------------------------------------------
    //! Similar to the C-file write method
    //!
    //! @param buffer
    //! @param size            number of bytes to be written
    //! @return                number of bytes actually write
    //------------------------------------------------------------------------
    virtual size_t write(unsigned char* buffer, size_t size) = 0;
    //------------------------------------------------------------------------
    //! Similar to the C file seek method
    //!
    //! @param offset         The offset you want to set
    //! @param whence         Some situations you can set: SEEK_CUR and SEEK_SET
    //------------------------------------------------------------------------
    virtual void seek(size_t offset, int whence) = 0;
    virtual void close() = 0;
    virtual size_t tell() = 0;
    bool IsRead() { return isread_; };
    bool IsWrite() { return iswrite_; };
    virtual bool IsEnd() = 0;

   protected:
    bool isread_;
    bool iswrite_;
};
