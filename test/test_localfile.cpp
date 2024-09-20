#include <gtest/gtest.h>
#include <cstdio>
#include <cstring>
#include <string>
#include "LocalFileHandler.h"

class LocalFileHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary file for testing
        tmpFileName = "test_local_file.tmp";
        FILE* tmpFile = fopen(tmpFileName.c_str(), "w");
        if (tmpFile) {
            fputs("Hello, World!", tmpFile);
            fclose(tmpFile);
        }
    }

    void TearDown() override {
        // Remove the temporary file
        std::remove(tmpFileName.c_str());
    }

    std::string tmpFileName;
};

TEST_F(LocalFileHandlerTest, ReadTest) {
    LocalFileHandler handler(tmpFileName, 'r');
    unsigned char buffer[20] = {0};
    size_t bytesRead = handler.read(buffer, sizeof(buffer));

    EXPECT_EQ(bytesRead, 13);
    EXPECT_STREQ(reinterpret_cast<char*>(buffer), "Hello, World!");
}

TEST_F(LocalFileHandlerTest, WriteTest) {
    LocalFileHandler handler(tmpFileName, 'w');
    const char* testData = "Test write operation";
    unsigned char* writeBuffer = reinterpret_cast<unsigned char*>(const_cast<char*>(testData));
    size_t bytesWritten = handler.write(writeBuffer, strlen(testData));

    EXPECT_EQ(bytesWritten, strlen(testData));

    // Verify the written content
    FILE* file = fopen(tmpFileName.c_str(), "r");
    ASSERT_NE(file, nullptr);
}

