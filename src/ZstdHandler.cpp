#include "ZstdHandler.h"
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include "CompressionHandler.h"
#include "FileHandler.h"
ZstdHandler::ZstdHandler(FileHandler& file_handler)
    : CompressionHandler(file_handler) {
    if (file_handler_.IsRead()) {
        dctx_ = ZSTD_createDStream();
        if (dctx_ == nullptr) {
            throw std::runtime_error("Failed to create ZSTD_DStream");
        }
        size_t const initResult = ZSTD_initDStream(dctx_);
        if (ZSTD_isError(initResult)) {
            throw "";
        }
        leftover_buffer_.resize(ZSTD_DStreamOutSize());

    } else {
        cctx_ = ZSTD_createCStream();
        write_buffer_.resize(ZSTD_DStreamOutSize());
    }
}


int ZstdHandler::WriteToFile(bool is_last) {
    std::vector<char> outputBuffer(ZSTD_CStreamOutSize());
    ZSTD_inBuffer inBuffer = {write_buffer_.data(), write_pos_, 0};
    ZSTD_outBuffer outBuffer = {outputBuffer.data(), outputBuffer.size(), 0};

    while (inBuffer.pos < inBuffer.size) {
        size_t const ret = ZSTD_compressStream(cctx_, &outBuffer, &inBuffer);
        if (ZSTD_isError(ret)) {
            return -1;
        }
        file_handler_.write(
            reinterpret_cast<unsigned char*>(outputBuffer.data()),
            outBuffer.pos);
        outBuffer.pos = 0;
    }
    /*
   * The final _write(), we have to call end_Stream
   */
    if (write_pos_ < write_buffer_.size()) {
        size_t const endRet = ZSTD_endStream(cctx_, &outBuffer);
        if (ZSTD_isError(endRet)) {
            return -1;
            // handle the error
        }
        file_handler_.write(
            reinterpret_cast<unsigned char*>(outputBuffer.data()),
            outBuffer.pos);  // In case we still have data
    }
    return 0;
}
size_t ZstdHandler::read(unsigned char* buffer, size_t size) {
    size_t totalRead = 0;
    if (leftover_size_ > 0) {
        size_t toCopy = std::min(leftover_size_, size);
        std::memcmp(buffer, leftover_buffer_.data() + leftover_pos_, toCopy);
        totalRead += toCopy;
        leftover_pos_ += toCopy;
        leftover_size_ -= toCopy;
        if (totalRead == size) {
            return totalRead;
        }
    }
    std::vector<char> inputBuffer(ZSTD_DStreamInSize());
    std::vector<char> outputBuffer(ZSTD_DStreamOutSize());
    ZSTD_inBuffer inBuffer = {inputBuffer.data(), 0, 0};
    ZSTD_outBuffer outBuffer = {outputBuffer.data(), outputBuffer.size(), 0};
    while (totalRead < size) {
        if (inBuffer.pos == inBuffer.size) {
            inBuffer.size = file_handler_.read(
                reinterpret_cast<unsigned char*>(inputBuffer.data()),
                inputBuffer.size());
            inBuffer.pos = 0;
            if (inBuffer.size == 0) {
                break;
            }
        }
        size_t const ret = ZSTD_decompressStream(dctx_, &outBuffer, &inBuffer);
        if (ZSTD_isError(ret)) {
            throw "";
        }
        size_t toCopy = std::min(outBuffer.pos, size - totalRead);
        std::memcpy(buffer + totalRead, outputBuffer.data(), toCopy);
        totalRead += toCopy;
        if (outBuffer.pos > toCopy) {
            leftover_size_ = outBuffer.pos - toCopy;
            std::memcpy(leftover_buffer_.data(), outputBuffer.data() + toCopy,
                        leftover_size_);
            leftover_pos_ = 0;
        } else {
            leftover_size_ = 0;
        }
        if (ret == 0 && inBuffer.pos == inBuffer.size) {
            break;
        }
    }
    return totalRead;
}


ZstdHandler::~ZstdHandler() {
    close();
}
void ZstdHandler::close() {
    if (is_closed_) {
        return;
    }
    if (file_handler_.IsRead()) {
        ZSTD_freeDStream(dctx_);
    } else {
        if (write_pos_ > 0) {
            WriteToFile(true);
        }
        ZSTD_freeCStream(cctx_);
    }
    file_handler_.close();
    is_closed_ = true;
}