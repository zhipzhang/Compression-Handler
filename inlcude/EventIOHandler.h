#pragma once

#include <memory>
#include <string>
#include "CompressionHandler.h"
#include "FileHandler.h"

/*
 * Try to provide the user_function for the eventio format.
 */
class EventIOHandler {
   public:
    EventIOHandler(const std::string& fname, const char mode,
                   const std::string& url = "root://eos01.ihep.ac.cn");
    /*
   * user_function defined in eventio_en.pdf
   */
    int user_function1(unsigned char* buffer, long bytes);
    int user_function2(unsigned char* buffer, long bytes);
    int user_function3(unsigned char* buffer, long bytes);
    int user_function4(unsigned char* buffer, long bytes);

   private:
    size_t read_(unsigned char* buffer, size_t bytes);
    size_t write_(unsigned char* buffer, size_t bytes);
    int seek_cur(size_t bytes);
    static bool ends_with(const std::string& str, const std::string& suffix);
    bool verifyCompressiontype_();
    std::unique_ptr<FileHandler> fileHandler_;
    std::unique_ptr<CompressionHandler> compressionHandler_;
};
