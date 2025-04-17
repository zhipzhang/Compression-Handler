#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "LocalFileHandler.h"
#include "ZstdHandler.h"

class ZstdHandlerTest {
protected:
    void SetUp() {
        tmpFileName = "test_zstd_file.tmp";
    }

    void TearDown() {
       // std::remove(tmpFileName.c_str());
    }

    std::string tmpFileName;
};

TEST_CASE_FIXTURE(ZstdHandlerTest, "WriteAndReadTest") {
    SetUp();
    const std::string testData = "Hello, Zstd compression!";

    // Write test
    {
        LocalFileHandler fileHandler(tmpFileName, 'w');
        ZstdHandler zstdHandler(fileHandler);
        
        size_t bytesWritten = zstdHandler.write(reinterpret_cast<unsigned char*>(const_cast<char*>(testData.c_str())), testData.length());
        zstdHandler.close();
        
        CHECK_EQ(bytesWritten, testData.length());
    }

    // Read test
    {
        LocalFileHandler fileHandler(tmpFileName, 'r');
        ZstdHandler zstdHandler(fileHandler);
        
        std::vector<unsigned char> readBuffer(100, 0);
        size_t bytesRead = zstdHandler.read(readBuffer.data(), readBuffer.size());

        CHECK_EQ(bytesRead, testData.length());
        CHECK_EQ(std::string(reinterpret_cast<char*>(readBuffer.data()), bytesRead), testData);
    }
    TearDown();
}

TEST_CASE_FIXTURE(ZstdHandlerTest, "LargeDataTest") {
    SetUp();
    const size_t largeDataSize = 1024 * 1024 ; // 1 MB
    const size_t largeDataSizeInBytes = largeDataSize * sizeof(int);
    std::vector<int> largeData(largeDataSize);
    for (long long i = 0; i < largeDataSize; ++i) {
        largeData[i] = static_cast<int> (i % (19999999));
    }

    // Write large data
    {
        LocalFileHandler fileHandler(tmpFileName, 'w');
        ZstdHandler zstdHandler(fileHandler);
        
        size_t bytesWritten = zstdHandler.write(reinterpret_cast<unsigned char*>(largeData.data()), largeDataSizeInBytes);
        zstdHandler.close();
        
        CHECK_EQ(bytesWritten, largeDataSizeInBytes);
    }

    // Read and verify large data
    {
        LocalFileHandler fileHandler(tmpFileName, 'r');
        ZstdHandler zstdHandler(fileHandler);
        
        std::vector<unsigned char> readBuffer(largeDataSizeInBytes);
        size_t bytesRead = zstdHandler.read(readBuffer.data(), readBuffer.size());

        CHECK_EQ(bytesRead, largeDataSizeInBytes);
        for(int i = 0; i < largeDataSize; ++i) {
            int read_value = *reinterpret_cast<int*>(readBuffer.data() + i * sizeof(int));
            CHECK_EQ(read_value, largeData[i]);
        }

    }
    TearDown();
}

