#include "GzipHandler.h"
#include <zconf.h>
#include <zlib.h>
#include <cstddef>

GzipHandler::GzipHandler(FileHandler& file_handler)
    : CompressionHandler(file_handler) {
    strm_ = static_cast<z_stream*>(std::malloc(sizeof(z_stream)));
    if (strm_ == nullptr) {
        throw "";
    }
    strm_->zalloc = Z_NULL;
    strm_->zfree = Z_NULL;
    strm_->opaque = Z_NULL;
    if (fileHandler_.IsRead()) {
        int ret = inflateInit2(strm_, 16 + MAX_WBITS);
        if (ret != Z_OK) {
            std::free(strm_);
            throw "";
        }
        leftoverBuffer_.resize(4096 * 5);
        unprocessedData_.resize(4096 * 5);
        leftoverSize_ = 0;
    } else {
        int ret = deflateInit2(strm_, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                               15 + 16, 8, Z_DEFAULT_STRATEGY);
        if (ret != Z_OK) {
            std::free(strm_);
        }
        writeBuffer_.resize(4096 * 5);
        writePos_ = 0;
    }
}

size_t GzipHandler::write(unsigned char* buffer, size_t size) {
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
int GzipHandler::_write() {
    std::vector<char> outputBuffer_(4096 * 10, 0);
    strm_->avail_in = writePos_;
    strm_->next_in = reinterpret_cast<Bytef*>(writeBuffer_.data());

    do {
        strm_->avail_out = outputBuffer_.size();
        strm_->next_out = reinterpret_cast<Bytef*>(outputBuffer_.data());
        int ret = deflate(strm_, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR) {
            return -1;
        }
        size_t have = outputBuffer_.size() - strm_->avail_out;
        fileHandler_.write(
            reinterpret_cast<unsigned char*>(outputBuffer_.data()), have);
    } while (strm_->avail_out == 0);
    return 0;
}
size_t GzipHandler::read(unsigned char* buffer, size_t size) {
    size_t totalRead = 0;
    if (leftoverSize_ > 0) {
        size_t toCopy = std::min(leftoverSize_, size);
        std::memcpy(buffer, leftoverBuffer_.data() + leftoverPos_, toCopy);
        totalRead += toCopy;
        leftoverPos_ += toCopy;
        leftoverSize_ -= toCopy;
        if (totalRead == size) {
            return totalRead;
        }
    }
    std::vector<char> inputBuffer_(4096, 0);
    std::vector<char> outputBuffer_(4096, 0);
    bool continueInflating = true;
    while (totalRead < size && continueInflating) {
        std::memcpy(inputBuffer_.data(), unprocessedData_.data(),
                    unprocessedSize_);
        size_t bytesread = fileHandler_.read(
            reinterpret_cast<unsigned char*>(inputBuffer_.data()) +
                unprocessedSize_,
            inputBuffer_.size() - unprocessedSize_);
        if (bytesread < inputBuffer_.size() - unprocessedSize_) {
            continueInflating = false;
        }
        strm_->avail_in = bytesread + unprocessedSize_;
        strm_->next_in = reinterpret_cast<Bytef*>(inputBuffer_.data());
        strm_->avail_out = size - totalRead;
        strm_->next_out = reinterpret_cast<Bytef*>(buffer + totalRead);
        int ret = inflate(strm_, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR ||
            ret == Z_MEM_ERROR) {
            throw "";
        }
        size_t bytesRead = size - totalRead - strm_->avail_out;
        totalRead += bytesRead;
        if (ret == Z_STREAM_END) {
            continueInflating = false;
        }
        if (strm_->avail_out == 0 && strm_->avail_in > 0) {
            unprocessedSize_ = strm_->avail_in;
            std::memcpy(unprocessedData_.data(), strm_->next_in,
                        unprocessedSize_);
            strm_->avail_in = 0;
        }
    }
    return totalRead;
}

GzipHandler::~GzipHandler() {
    if (fileHandler_.IsRead()) {
        inflateEnd(strm_);
    } else {
        // make sure no data is left.
        _write();
        deflateEnd(strm_);
    }
    std::free(strm_);
}
