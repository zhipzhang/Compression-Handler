#include <gtest/gtest.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "LocalFileHandler.h"
#include "ZstdHandler.h"

class ZstdHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        tmpFileName = "test_zstd_file.tmp";
    }

    void TearDown() override {
        std::remove(tmpFileName.c_str());
    }

    std::string tmpFileName;
};

TEST_F(ZstdHandlerTest, WriteAndReadTest) {
    const std::string testData = "Hello, Zstd compression!";

    // Write test
    {
        LocalFileHandler fileHandler(tmpFileName, 'w');
        ZstdHandler zstdHandler(fileHandler);
        
        size_t bytesWritten = zstdHandler.write(reinterpret_cast<unsigned char*>(const_cast<char*>(testData.c_str())), testData.length());
        zstdHandler.close();
        
        EXPECT_EQ(bytesWritten, testData.length());
    }

    // Read test
    {
        LocalFileHandler fileHandler(tmpFileName, 'r');
        ZstdHandler zstdHandler(fileHandler);
        
        std::vector<unsigned char> readBuffer(100, 0);
        size_t bytesRead = zstdHandler.read(readBuffer.data(), readBuffer.size());

        EXPECT_EQ(bytesRead, testData.length());
        EXPECT_EQ(std::string(reinterpret_cast<char*>(readBuffer.data()), bytesRead), testData);
    }
}

TEST_F(ZstdHandlerTest, LargeDataTest) {
    const size_t largeDataSize = 1024 * 1024; // 1 MB
    std::vector<unsigned char> largeData(largeDataSize);
    for (size_t i = 0; i < largeDataSize; ++i) {
        largeData[i] = static_cast<unsigned char>(i % 256);
    }

    // Write large data
    {
        LocalFileHandler fileHandler(tmpFileName, 'w');
        ZstdHandler zstdHandler(fileHandler);
        
        size_t bytesWritten = zstdHandler.write(largeData.data(), largeData.size());
        zstdHandler.close();
        
        EXPECT_EQ(bytesWritten, largeData.size());
    }

    // Read and verify large data
    {
        LocalFileHandler fileHandler(tmpFileName, 'r');
        ZstdHandler zstdHandler(fileHandler);
        
        std::vector<unsigned char> readBuffer(largeDataSize);
        size_t bytesRead = zstdHandler.read(readBuffer.data(), readBuffer.size());

        EXPECT_EQ(bytesRead, largeData.size());
        EXPECT_EQ(readBuffer, largeData);
    }
}

TEST_F(ZstdHandlerTest, MultipleWritesTest) {
    LocalFileHandler fileHandler(tmpFileName, 'w');
    ZstdHandler zstdHandler(fileHandler);

    const std::string data1 = "First chunk of data. ";
    const std::string data2 = "Second chunk of data. ";
    const std::string data3 = "Third chunk of data.";

    zstdHandler.write(reinterpret_cast<unsigned char*>(const_cast<char*>(data1.c_str())), data1.length());
    zstdHandler.write(reinterpret_cast<unsigned char*>(const_cast<char*>(data2.c_str())), data2.length());
    zstdHandler.write(reinterpret_cast<unsigned char*>(const_cast<char*>(data3.c_str())), data3.length());
    zstdHandler.close();

    // Read and verify
    LocalFileHandler readFileHandler(tmpFileName, 'r');
    ZstdHandler readZstdHandler(readFileHandler);

    std::vector<unsigned char> readBuffer(100);
    size_t totalBytesRead = 0;
    std::string result;

    while (true) {
        size_t bytesRead = readZstdHandler.read(readBuffer.data(), readBuffer.size());
        if (bytesRead == 0) break;
        result.append(reinterpret_cast<char*>(readBuffer.data()), bytesRead);
        totalBytesRead += bytesRead;
    }

    EXPECT_EQ(result, data1 + data2 + data3);
    EXPECT_EQ(totalBytesRead, data1.length() + data2.length() + data3.length());
}
