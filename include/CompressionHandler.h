#pragma once
#include <vector>
#include "FileHandler.h"
/**
 * Base class for compressed format handler.
 */
class CompressionHandler {
   public:
    virtual ~CompressionHandler() = default;
    virtual size_t read(unsigned char* buffer, size_t size) = 0;

    /**
     * Write the buffer to the write_buffer_.
     * @param buffer The data to write
     * @param size The size of the data
     * @return The number of bytes written
     */
    size_t write(unsigned char* buffer, size_t size);
    virtual void close() = 0;
    virtual int WriteToFile(bool is_last = false) = 0;
   protected:
    FileHandler& file_handler_;
    explicit CompressionHandler(FileHandler& file_handler)
        : file_handler_(file_handler) {}

    // Handle the leftover decompressed data
    std::vector<char> leftover_buffer_;
    size_t leftover_size_ = 0;
    size_t leftover_pos_ = 0;

    // Store the compressed data first
    std::vector<char> write_buffer_;
    size_t write_pos_ = 0;
};
