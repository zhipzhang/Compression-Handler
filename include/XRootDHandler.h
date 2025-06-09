#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <fcntl.h>
#include "FileHandler.h"
#define _FILE_OFFSET_BITS 64
#include "XrdPosix/XrdPosixExtern.hh"
class XRootDFileHandler : public FileHandler {
   public:
    XRootDFileHandler(const std::string& filepath, const char mode = 'r') {
        if (mode == 'r') {
            xrootd_fd = XrdPosix_Open(filepath.c_str(), O_RDONLY);
            isread_ = true;
            iswrite_ = false;
        } else {
            xrootd_fd = XrdPosix_Open(filepath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            iswrite_ = true;
            isread_ = false;
        }
        if(xrootd_fd == -1)
        {
            throw std::runtime_error("Failed to open file");
        }
    }

    size_t write(unsigned char* buffer, size_t size) override {
        return XrdPosix_Write(xrootd_fd, buffer, size);
    }

    size_t read(unsigned char* buffer, size_t size) override {
        return XrdPosix_Read(xrootd_fd, buffer, size);
    }

    void seek(size_t offset, int whence) override {
        XrdPosix_Lseek(xrootd_fd, offset, whence);
    }

    size_t tell() override { 
        return XrdPosix_Lseek(xrootd_fd, 0, SEEK_CUR);
    }

    bool IsEnd() override { 
        if (xrootd_fd < 0) return true;
        off_t current = XrdPosix_Lseek(xrootd_fd, 0, SEEK_CUR);
        off_t end = XrdPosix_Lseek(xrootd_fd, 0, SEEK_END);
        XrdPosix_Lseek(xrootd_fd, current, SEEK_SET);
        return current >= end;
    }

    void close() override { 
        if (xrootd_fd >= 0) {
            XrdPosix_Close(xrootd_fd);
            xrootd_fd = -1;
        }
    }

    ~XRootDFileHandler() {
        close();
    }

   private:
    int xrootd_fd = -1;
};
