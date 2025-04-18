#include "GzipHandler.h"
#include <zconf.h>
#include <zlib.h>
#include <cstddef>
#include <stdexcept>

GzipHandler::GzipHandler(FileHandler& file_handler)
    : CompressionHandler(file_handler) {
    strm_ = static_cast<z_stream*>(std::malloc(sizeof(z_stream)));
    if (strm_ == nullptr) {
        throw "";
    }
    strm_->zalloc = Z_NULL;
    strm_->zfree = Z_NULL;
    strm_->opaque = Z_NULL;
    if (file_handler_.IsRead()) {
        int ret = inflateInit2(strm_, 16 + MAX_WBITS);
        if (ret != Z_OK) {
            std::free(strm_);
            throw "";
        }
        leftover_buffer_.resize(4096 * 5);
        unprocessed_data_.resize(4096 * 5);
        unprocessed_size_ = 0;
        leftover_size_ = 0;
    } else {
        int ret = deflateInit2(strm_, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                               15 + 16, 8, Z_DEFAULT_STRATEGY);
        if (ret != Z_OK) {
            std::free(strm_);
        }
        write_buffer_.resize(4096 * 5);
        write_pos_ = 0;
    }
}
int GzipHandler::WriteToFile(bool is_last) {
    std::vector<char> output_buffer_(4096 * 10, 0);
    strm_->avail_in = write_pos_;
    strm_->next_in = reinterpret_cast<Bytef*>(write_buffer_.data());

    do {
        strm_->avail_out = output_buffer_.size();
        strm_->next_out = reinterpret_cast<Bytef*>(output_buffer_.data());
        int ret = deflate(strm_, is_last ? Z_FINISH : Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR ||
            ret == Z_MEM_ERROR || ret == Z_BUF_ERROR) {
            return -1;
        }
        size_t have = output_buffer_.size() - strm_->avail_out;
        if (have > 0) {
            file_handler_.write(
                reinterpret_cast<unsigned char*>(output_buffer_.data()), have);
        }
    } while (strm_->avail_out == 0);
    return 0;
}
size_t GzipHandler::read(unsigned char* buffer, size_t size) {
    size_t totalRead = 0;
    if (leftover_size_ > 0) {
        size_t toCopy = std::min(leftover_size_, size);
        std::memcpy(buffer, leftover_buffer_.data() + leftover_pos_, toCopy);
        totalRead += toCopy;
        leftover_pos_ += toCopy;
        leftover_size_ -= toCopy;
        if (totalRead == size) {
            return totalRead;
        }
    }

    std::vector<char> outputBuffer_(4096, 0);
    bool continueInflating = true;
    while (totalRead < size && continueInflating) {
        std::memcpy(input_buffer_.data(), unprocessed_data_.data(),
                    unprocessed_size_);
        size_t bytesread = file_handler_.read(
            reinterpret_cast<unsigned char*>(input_buffer_.data()) +
                unprocessed_size_,
            input_buffer_.size() - unprocessed_size_);
        
        // End of the compressed file.
        if (bytesread < input_buffer_.size() - unprocessed_size_) {
            IsReadEnd = true;
            continueInflating = false;
        }
        strm_->avail_in = bytesread + unprocessed_size_;
        strm_->next_in = reinterpret_cast<Bytef*>(input_buffer_.data());
        strm_->avail_out = size - totalRead;
        strm_->next_out = reinterpret_cast<Bytef*>(buffer + totalRead);
        int ret = inflate(strm_, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR ||
            ret == Z_MEM_ERROR) {
            throw "";
        }
        size_t bytesDecompressed = size - totalRead - strm_->avail_out;
        totalRead += bytesDecompressed;
        if (ret == Z_STREAM_END) {
            // Make sure all data is decompressed.
            int ret = inflate(strm_, Z_FINISH);
            if (ret == Z_STREAM_ERROR) {
                throw std::runtime_error("Error in inflate");
            }
            continueInflating = false;
        }
        if (strm_->avail_in > 0) {
            unprocessed_size_ = strm_->avail_in;
            std::memcpy(unprocessed_data_.data(), strm_->next_in,
                        unprocessed_size_);
            strm_->avail_in = 0;
        } else {
            unprocessed_size_ = 0;
        }
    }
    if (strm_->avail_out == 0 && continueInflating) {
        auto avail_size = leftover_buffer_.size();
        size_t nextout_pos = 0;
        do {
            strm_->avail_out = avail_size;
            strm_->next_out =
                reinterpret_cast<Bytef*>(leftover_buffer_.data() + nextout_pos);
            int ret = inflate(strm_, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR ||
                ret == Z_MEM_ERROR) {
                throw std::runtime_error("Error in inflate");
            }
            leftover_pos_ += avail_size - strm_->avail_out;
            if (strm_->avail_out > 0) {
                // All data is decompressed.
                break;
            }
            nextout_pos = avail_size - strm_->avail_out;
            leftover_buffer_.resize(leftover_buffer_.size() + avail_size);
        } while (strm_->avail_out == 0);
    }
    return totalRead;
}

void GzipHandler::cleanup() {
    if (is_closed_) {
        return;
    }
    if (file_handler_.IsRead()) {
        inflateEnd(strm_);
    } else {
        // make sure no data is left.
        WriteToFile(true);
        deflateEnd(strm_);
    }
    file_handler_.close();
    is_closed_ = true;
}
void GzipHandler::close() {
    cleanup();
}
GzipHandler::~GzipHandler() {
    cleanup();
    std::free(strm_);
}