#pragma once
#include <zstd.h>
#include <cstdio>
#include <cstring>
#include "CompressionHandler.h"
#include "FileHandler.h"
class ZstdHandler : public CompressionHandler {
   public:
    ZstdHandler(FileHandler& file_handler);
    ~ZstdHandler();
    size_t read(unsigned char* buffer, size_t size) override;
    void close() override;

   private:

    void read_compressed_from_file();
    bool is_closed_ = false;
    int WriteToFile(bool is_last = false) override;
    ZSTD_DStream* dctx_ = nullptr;  // decompression context
    ZSTD_CStream* cctx_ = nullptr;  // compression context
};
