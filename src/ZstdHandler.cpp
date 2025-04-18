#include "ZstdHandler.h"
#include <cstddef>
#include <cstdlib>
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
            throw std::runtime_error("Failed to initialize ZSTD_DStream");
        }
        leftover_buffer_.resize(ZSTD_DStreamOutSize());
        input_buffer_ = std::vector<char>(ZSTD_DStreamInSize(), 0);
    } else {
        cctx_ = ZSTD_createCStream();
        write_buffer_.resize(ZSTD_CStreamOutSize());
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
        std::memcpy(buffer, leftover_buffer_.data() + leftover_pos_, toCopy);
        totalRead += toCopy;
        leftover_pos_ += toCopy;
        leftover_size_ -= toCopy;
        if (totalRead == size) {
            return totalRead;
        }
    }
    // decompressed_buffer_ is used up, we need to read more data or decompress more data.
    ZSTD_inBuffer inBuffer;

    // Make sure the inBuffer have data.
    if(input_pos_ < input_size_) {
        inBuffer = {input_buffer_.data(), input_size_, input_pos_};
    }
    else
    {
        read_compressed_from_file();
        inBuffer = {input_buffer_.data(), input_size_, input_pos_};
    }

    // Save the decompressed data to the outBuffer firstly, it have leftover data copied to the leftover_buffer_.
    std::vector<char> output_buffer(ZSTD_DStreamOutSize());
    ZSTD_outBuffer outBuffer = {output_buffer.data(), output_buffer.size(), 0};
    while (totalRead < size) {

        if(IsReadEnd)
        {
            return totalRead;
        }

        // If the inBuffer is used up, read more data from the file.
        if (inBuffer.pos == inBuffer.size) {
            read_compressed_from_file();
            inBuffer = {input_buffer_.data(), input_size_, input_pos_};
        }
        // Each time before decompress, we need to reset the outBuffer.
        outBuffer.pos = 0;

        size_t const ret = ZSTD_decompressStream(dctx_, &outBuffer, &inBuffer);
        if (ZSTD_isError(ret)) {
            throw std::runtime_error("ZSTD_decompressStream failed: " + std::string(ZSTD_getErrorName(ret)));
        }

        // Update the input buffer position.
        input_pos_ = inBuffer.pos;

        // Copy the decompressed data to the output buffer.
        size_t toCopy = std::min(outBuffer.pos, size - totalRead);
        std::memcpy(buffer + totalRead, outBuffer.dst, toCopy);
        totalRead += toCopy;

        // If there is leftover data, copy it to the leftover buffer.
        if (outBuffer.pos > toCopy) {
            leftover_size_ = outBuffer.pos - toCopy;
            std::memcpy(leftover_buffer_.data(), (unsigned char*)outBuffer.dst + toCopy,
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


void ZstdHandler::read_compressed_from_file() {
    int read_bytes = file_handler_.read(reinterpret_cast<unsigned char*>(input_buffer_.data()), input_buffer_.size());
    input_size_ = read_bytes;
    input_pos_ = 0;
    if(read_bytes < input_buffer_.size()) {
        // We have reached the end of the file.
        IsReadEnd = true;
    }
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