#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <cstdio>
#include <cstring>
#include <string>
#include "LocalFileHandler.h"

class LocalFileHandlerTest {
protected:
    void SetUp() {
        // Create a temporary file for testing
        tmpFileName = "test_localfile.tmp";
    }

    void TearDown() {
        // Remove the temporary file
    }

    std::string tmpFileName;
};

TEST_CASE_FIXTURE(LocalFileHandlerTest, "WriteAndReadTest") {
    SetUp();
    // Write test
    {
        LocalFileHandler handler(tmpFileName, 'w');
        const char* testData = "Hello, World!";
        unsigned char* writeBuffer = reinterpret_cast<unsigned char*>(const_cast<char*>(testData));
        size_t bytesWritten = handler.write(writeBuffer, strlen(testData));
        handler.close();
        CHECK_EQ(bytesWritten, strlen(testData));
    }
    // Read test
    {
        LocalFileHandler handler(tmpFileName, 'r');
        unsigned char buffer[20] = {0};
        size_t bytesRead = handler.read(buffer, sizeof(buffer));
        handler.close();
        CHECK_EQ(bytesRead, 13);
        CHECK_EQ(std::string(reinterpret_cast<char*>(buffer)), "Hello, World!");
    }
    TearDown();
}


