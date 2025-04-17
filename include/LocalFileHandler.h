#pragma once
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sys/stat.h>
#include "FileHandler.h"
class LocalFileHandler : public FileHandler {
   public:
    LocalFileHandler(const std::string& filepath, const char mode = 'r') {
        if (mode == 'r') {
            fd_ = open(filepath.c_str(), O_RDONLY);
            isread_ = true;
            iswrite_ = false;
        } else {
            fd_ = open(filepath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            iswrite_ = true;
            isread_ = false;
        }
    }
    size_t read(unsigned char* buffer, size_t size) override {
        return ::read(fd_, buffer, size);
    }
    size_t write(unsigned char* buffer, size_t size) override {
        return ::write(fd_, buffer, size);
    }
    void seek(size_t offset, int whence) override {
        lseek(fd_, offset, whence);
    }
    void close() override { 
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }
    size_t tell() override { 
        return lseek(fd_, 0, SEEK_CUR); 
    }
    bool IsEnd() override { 
        if (fd_ < 0) return true;
        off_t current = lseek(fd_, 0, SEEK_CUR);
        off_t end = lseek(fd_, 0, SEEK_END);
        lseek(fd_, current, SEEK_SET);
        return current >= end;
    }
    ~LocalFileHandler() {
        close();
    }

   private:
    int fd_ = -1;
};
