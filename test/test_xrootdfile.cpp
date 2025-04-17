#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <cstring>
#include <string>
#include "XRootDHandler.h"

class XRootDFileHandlerTest {
protected:
    void SetUp() {
        // Set up a test file on a XRootD server
        // Note: You need to replace this with a valid XRootD URL and ensure you have write access
        testFileUrl = "root://127.0.0.1:1094//tmp/test_xrootd_file.tmp";
    }

    void TearDown() {
        // Clean up: remove the test file if it exists
        // Note: This requires additional XRootD operations to delete a file
    }

    std::string testFileUrl;
};

TEST_CASE_FIXTURE(XRootDFileHandlerTest, "WriteAndReadTest") {
    SetUp();
    // Write test
    {
        XRootDFileHandler handler(testFileUrl, 'w');
        const char* testData = "Hello, XRootD!";
        unsigned char* writeBuffer = reinterpret_cast<unsigned char*>(const_cast<char*>(testData));
        size_t bytesWritten = handler.write(writeBuffer, strlen(testData));
        handler.close();
        CHECK_EQ(bytesWritten, strlen(testData));
    }

    // Read test
    {
        XRootDFileHandler handler(testFileUrl, 'r');
        unsigned char buffer[20] = {0};
        size_t bytesRead = handler.read(buffer, sizeof(buffer));

        CHECK_EQ(bytesRead, 14); // "Hello, XRootD!" is 14 bytes
        CHECK_EQ(std::string(reinterpret_cast<char*>(buffer)), "Hello, XRootD!");
    }
    TearDown();
}

TEST_CASE_FIXTURE(XRootDFileHandlerTest, "SeekAndTellTest") {
    SetUp();
    XRootDFileHandler handler(testFileUrl, 'r');
    
    // Seek to a specific position
    handler.seek(7, SEEK_SET);
    CHECK_EQ(handler.tell(), 7);

    // Read from that position
    unsigned char buffer[10] = {0};
    size_t bytesRead = handler.read(buffer, sizeof(buffer));

    CHECK_EQ(bytesRead, 7); // "XRootD!" is 7 bytes
    CHECK_EQ(std::string(reinterpret_cast<char*>(buffer)), "XRootD!");
    TearDown();
}

TEST_CASE_FIXTURE(XRootDFileHandlerTest, "IsEndTest") {
    SetUp();
    XRootDFileHandler handler(testFileUrl, 'r');
    
    // Read entire file
    unsigned char buffer[20] = {0};
    size_t bytesRead = handler.read(buffer, sizeof(buffer));

    // Try to read more, should be at end
    unsigned char extraBuffer[5] = {0};
    size_t bytesRead2 = handler.read(extraBuffer, sizeof(extraBuffer));

    CHECK_EQ(bytesRead2, 0);
    CHECK(handler.IsEnd());
    TearDown();
}

