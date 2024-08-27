#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include "FileHandler.h"
#include "XrdCl/XrdClFile.hh"

class XRootDFileHandler : public FileHandler {
   public:
    XRootDFileHandler(const std::string& filepath, const char mode = 'r');

    size_t write(unsigned char* buffer, size_t size) override;
    size_t read(unsigned char* buffer, size_t size) override;

    void seek(size_t offset, int whence) override;

    size_t tell() override { return static_cast<size_t>(current_offset_); }
    bool IsEnd() override;

   private:
    void GetfileSize_();
    std::shared_ptr<XrdCl::File> file_;
    uint64_t current_offset_;
    uint64_t fileSize_;
};
