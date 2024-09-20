#include "CompressionHandler.h"
#include <cstring>
#include <stdexcept>
size_t CompressionHandler::write(unsigned char* buffer , size_t size)
{
        size_t totalWrite = 0;
    while (totalWrite < size) {
        size_t toWrite =
            std::min(size - totalWrite, write_buffer_.size() - write_pos_);
        std::memcpy(write_buffer_.data() + write_pos_, buffer + totalWrite,
                    toWrite);
        write_pos_ += toWrite;
        totalWrite += toWrite;

        if (write_pos_ == write_buffer_.size()) {
            int ret = WriteToFile();
            if (ret != 0) {
                throw std::runtime_error("Error in deflate");
            }
            write_pos_ = 0;
            continue;
        }
    }
    return totalWrite;
}
