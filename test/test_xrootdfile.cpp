#include <gtest/gtest.h>
#include <cstring>
#include <string>
#include "XRootDHandler.h"

class XRootDFileHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up a test file on a XRootD server
        // Note: You need to replace this with a valid XRootD URL and ensure you have write access
        testFileUrl = "root://127.0.0.1:1094//tmp/test_xrootd_file.tmp";
    }

    void TearDown() override {
        // Clean up: remove the test file if it exists
        // Note: This requires additional XRootD operations to delete a file
    }

    std::string testFileUrl;
};

TEST_F(XRootDFileHandlerTest, WriteAndReadTest) {
    // Write test
    {
        XRootDFileHandler handler(testFileUrl, 'w');
        const char* testData = "Hello, XRootD!";
        unsigned char* writeBuffer = reinterpret_cast<unsigned char*>(const_cast<char*>(testData));
        size_t bytesWritten = handler.write(writeBuffer, strlen(testData));
        handler.close();
        EXPECT_EQ(bytesWritten, strlen(testData));
    }

    // Read test
    {
        XRootDFileHandler handler(testFileUrl, 'r');
        unsigned char buffer[20] = {0};
        size_t bytesRead = handler.read(buffer, sizeof(buffer));

        EXPECT_EQ(bytesRead, 14); // "Hello, XRootD!" is 14 bytes
        EXPECT_STREQ(reinterpret_cast<char*>(buffer), "Hello, XRootD!");
    }
}

TEST_F(XRootDFileHandlerTest, SeekAndTellTest) {
    XRootDFileHandler handler(testFileUrl, 'r');
    
    // Seek to a specific position
    handler.seek(7, SEEK_SET);
    EXPECT_EQ(handler.tell(), 7);

    // Read from that position
    unsigned char buffer[10] = {0};
    size_t bytesRead = handler.read(buffer, sizeof(buffer));

    EXPECT_EQ(bytesRead, 7); // "XRootD!" is 7 bytes
    EXPECT_STREQ(reinterpret_cast<char*>(buffer), "XRootD!");
}

TEST_F(XRootDFileHandlerTest, IsEndTest) {
    XRootDFileHandler handler(testFileUrl, 'r');
    
    // Read entire file
    unsigned char buffer[20] = {0};
    handler.read(buffer, sizeof(buffer));

    // Try to read more, should be at end
    unsigned char extraBuffer[5] = {0};
    size_t bytesRead = handler.read(extraBuffer, sizeof(extraBuffer));

    EXPECT_EQ(bytesRead, 0);
    EXPECT_TRUE(handler.IsEnd());
}

