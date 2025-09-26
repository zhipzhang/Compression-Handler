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
    void close() override;

   private:
    int WriteToFile(bool is_last = false) override;
    void handleLeftoverDecompression();
    /* can be used for compress or decompress */
    z_stream* strm_;
    /* the unprocessed compressed data : needed for gzip handler*/
    std::vector<char> unprocessed_data_;
    size_t unprocessed_size_ = 0;

    bool is_closed_ = false;
    void cleanup();
};
