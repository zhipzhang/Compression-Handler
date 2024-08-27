#include <cstddef>
#include <cstdint>
#include <memory>
#include "XRootDHandler.h"
#include "XrdCl/XrdClFileSystem.hh"
#include "XrdCl/XrdClXRootDResponses.hh"

XRootDFileHandler::XRootDFileHandler(const std::string& filepath,
                                     const char mode) {
    file_ = std::make_shared<XrdCl::File>();
    XrdCl::XRootDStatus flag;
    if (mode == 'r') {
        flag = file_->Open(filepath, XrdCl::OpenFlags::Read);
        isread_ = true;
        iswrite_ = false;
        GetfileSize_();
    } else {
        flag = file_->Open(filepath, XrdCl::OpenFlags::Write);
        isread_ = false;
        iswrite_ = true;
    }
};

size_t XRootDFileHandler::write(unsigned char* buffer, size_t size) {
    if (size <= 0) {
        return 0;
    }
    XrdCl::XRootDStatus status;
    uint32_t bytesWrite;
    status = file_->Write(current_offset_, size, static_cast<void*>(buffer));
    if (!status.IsOK()) {}
    current_offset_ += bytesWrite;
    return static_cast<size_t>(bytesWrite);
}
size_t XRootDFileHandler::read(unsigned char* buffer, size_t size) {
    XrdCl::XRootDStatus status;
    uint32_t bytesRead;
    status = file_->Read(current_offset_, static_cast<uint32_t>(size),
                         static_cast<void*>(buffer), bytesRead);
    if (!status.IsOK()) {
        throw std::runtime_error("Failed to read Xrootd file !");
    }
    current_offset_ += bytesRead;
    return static_cast<size_t>(bytesRead);
}
void XRootDFileHandler::seek(size_t offset, int whence) {
    XrdCl::XRootDStatus status;
    uint64_t newPosition;
    switch (whence) {
        case SEEK_SET:
            current_offset_ = offset;
            break;
        case SEEK_CUR:
            current_offset_ += offset;
            break;
        default:
            throw std::invalid_argument("Invalid Argument for whence");
    }
}
bool XRootDFileHandler::IsEnd() {
    if (current_offset_ >= fileSize_) {
        return true;
    } else {
        return false;
    }
}
void XRootDFileHandler::GetfileSize_() {
    XrdCl::XRootDStatus status;
    XrdCl::StatInfo* statinfo = nullptr;
    status = file_->Stat(false, statinfo);
    if (!status.IsOK() || !statinfo) {
        ;
    }
    fileSize_ = statinfo->GetSize();
}
