#include "ZstdHandler.h"
#include <cstddef>
#include <cstring>
#include "CompressionHandler.h"
#include "FileHandler.h"
ZstdHandler::ZstdHandler(FileHandler& file_handler)
    : CompressionHandler(file_handler) {
    if (fileHandler_.IsRead()) {
        dctx_ = ZSTD_createDStream();
        if (dctx_ == nullptr) {
            throw std::runtime_error("Failed to create ZSTD_DStream");
        }
        size_t const initResult = ZSTD_initDStream(dctx_);
        if (ZSTD_isError(initResult)) {
            throw "";
        }
        leftoverBuffer_.resize(ZSTD_DStreamOutSize());

    } else {
        cctx_ = ZSTD_createCStream();
        writeBuffer_.resize(ZSTD_DStreamOutSize());
    }
}

size_t ZstdHandler::write(unsigned char* buffer, size_t size) {
    size_t totalWrite = 0;
    size_t toWrite = std::min(size, writeBuffer_.size() - writePos_);
    std::memcpy(writeBuffer_.data() + writePos_, buffer, toWrite);
    if (toWrite < size) {
        _write();
        writePos_ = 0;
        toWrite += write(buffer + toWrite, size - toWrite);
    }
    return toWrite;
}

int ZstdHandler::_write() {
    std::vector<char> outputBuffer(ZSTD_CStreamOutSize());
    ZSTD_inBuffer inBuffer = {writeBuffer_.data(), writePos_, 0};
    ZSTD_outBuffer outBuffer = {outputBuffer.data(), outputBuffer.size(), 0};

    while (inBuffer.pos < inBuffer.size) {
        size_t const ret = ZSTD_compressStream(cctx_, &outBuffer, &inBuffer);
        if (ZSTD_isError(ret)) {
            return -1;
        }
        fileHandler_.write(
            reinterpret_cast<unsigned char*>(outputBuffer.data()),
            outBuffer.pos);
        outBuffer.pos = 0;
    }
    /*
   * The final _write(), we have to call end_Stream
   */
    if (writePos_ < writeBuffer_.size()) {
        size_t const endRet = ZSTD_endStream(cctx_, &outBuffer);
        if (ZSTD_isError(endRet)) {
            return -1;
            // handle the error
        }
        fileHandler_.write(
            reinterpret_cast<unsigned char*>(outputBuffer.data()),
            outBuffer.pos);  // In case we still have data
    }
    return 0;
}
size_t ZstdHandler::read(unsigned char* buffer, size_t size) {
    size_t totalRead = 0;
    if (leftoverSize_ > 0) {
        size_t toCopy = std::min(leftoverSize_, size);
        std::memcmp(buffer, leftoverBuffer_.data() + leftoverPos_, toCopy);
        totalRead += toCopy;
        leftoverPos_ += toCopy;
        leftoverSize_ -= toCopy;
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
            inBuffer.size = fileHandler_.read(
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
            leftoverSize_ = outBuffer.pos - toCopy;
            std::memcpy(leftoverBuffer_.data(), outputBuffer.data() + toCopy,
                        leftoverSize_);
            leftoverPos_ = 0;
        } else {
            leftoverSize_ = 0;
        }
        if (ret == 0 && inBuffer.pos == inBuffer.size) {
            break;
        }
    }
    return totalRead;
}
ZstdHandler::~ZstdHandler() {
    if (writePos_ > 0) {
        _write();
    }
    ZSTD_freeDStream(dctx_);
    ZSTD_freeCStream(cctx_);
}
