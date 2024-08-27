#pragma once
#include <vector>
#include "FileHandler.h"

/*
 * Base Class for compressed format handler.
 */

class CompressionHandler {
   public:
    virtual ~CompressionHandler() = default;
    virtual size_t read(unsigned char* buffer, size_t size) = 0;
    //------------------------------------------------------------------------
    //! write the buffer to the writeBuffer_
    //!
    //! @param buffer     
    //! @param size     
    //! @return        return_description
    //------------------------------------------------------------------------
    virtual size_t write(unsigned char* buffer, size_t size) = 0;

   protected:
    //------------------------------------------------------------------------
    //! write the data in the writeBuffer_ to the file
    //!
    //! @return        0(OK), -1(Bad)
    //------------------------------------------------------------------------
    virtual int _write() = 0;
    FileHandler& fileHandler_;
    CompressionHandler(FileHandler& fileHandler) : fileHandler_(fileHandler){};
    /* Handle the leftover decompressed data */
    std::vector<char> leftoverBuffer_;
    size_t leftoverSize_ = 0;
    size_t leftoverPos_ = 0;
    /* Store the compressed data first */
    std::vector<char> writeBuffer_;
    size_t writePos_ = 0;
};
