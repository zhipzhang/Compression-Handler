#pragma once
#include <fstream>
#include <ios>
#include "FileHandler.h"
class LocalFileHandler : public FileHandler {
   public:
    LocalFileHandler(const std::string& filepath, const char mode = 'r') {
        if (mode == 'r') {
            file_.open(filepath, std::ios::in | std::ios::binary);
            isread_ = true;
            iswrite_ = false;
        } else {
            file_.open(filepath, std::ios::out | std::ios::binary);
            iswrite_ = true;
            isread_ = false;
        }
    }
    size_t read(unsigned char* buffer, size_t size) override {
        file_.read(reinterpret_cast<char*>(buffer), size);
        return file_.gcount();
    }
    size_t write(unsigned char* buffer, size_t size) override {
        file_.write(reinterpret_cast<const char*>(buffer), size);
        return size;
    }
    void seek(size_t offset, int whence) override {
        file_.seekg(offset, static_cast<std::ios_base::seekdir>(whence));
    }
    void close() override { file_.close(); }
    size_t tell() override { return file_.tellg(); }
    bool IsEnd() override { return file_.eof(); }

   private:
    std::fstream file_;
};
