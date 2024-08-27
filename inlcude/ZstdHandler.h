#pragma once
#include <zstd.h>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <vector>
#include "CompressionHandler.h"
#include "FileHandler.h"
class ZstdHandler : public CompressionHandler {
   public:
    ZstdHandler(FileHandler& file_handler);
    ~ZstdHandler();
    size_t read(unsigned char* buffer, size_t size) override;
    size_t write(unsigned char* buffer, size_t size) override;

   private:
    int _write() override;
    ZSTD_DStream* dctx_;  // decompression context
    ZSTD_CStream* cctx_;  //
};
