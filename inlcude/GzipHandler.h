#pragma once
#include <zconf.h>
#include <zlib.h>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "CompressionHandler.h"
#include "FileHandler.h"

class GzipHandler : public CompressionHandler {
   public:
    GzipHandler(FileHandler& file_handler);
    ~GzipHandler();
    size_t read(unsigned char* buffer, size_t size) override;
    size_t write(unsigned char* buffer, size_t size) override;

   private:
    int _write() override;
    /* can be used for compress or decompress */
    z_stream* strm_;
    /* the unprocessed compressed data : needed for gzip handler*/
    std::vector<char> unprocessedData_;
    size_t unprocessedSize_ = 0;
};
