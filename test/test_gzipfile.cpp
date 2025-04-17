#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "LocalFileHandler.h"
#include "GzipHandler.h"

class GzipHandlerTest {
protected:
    void SetUp() {
        tmpFileName = "test_gzip_file.gz";
    }

    void TearDown() {
        // std::remove(tmpFileName.c_str());
    }

    std::string tmpFileName;
};

TEST_CASE_FIXTURE(GzipHandlerTest, "WriteAndReadTest") {
    SetUp();
    const std::string testData = "Hello, Gzip compression!";
    
    // Write test
    {
        LocalFileHandler fileHandler(tmpFileName, 'w');
        GzipHandler gzipHandler(fileHandler);
        
        size_t bytesWritten = gzipHandler.write(reinterpret_cast<unsigned char*>(const_cast<char*>(testData.c_str())), testData.length());
        gzipHandler.close();
        
        CHECK_EQ(bytesWritten, testData.length());
    }

    // Read test
    {
        LocalFileHandler fileHandler(tmpFileName, 'r');
        GzipHandler gzipHandler(fileHandler);
        
        std::vector<unsigned char> readBuffer(100, 0);
        size_t bytesRead = gzipHandler.read(readBuffer.data(), readBuffer.size());

        CHECK_EQ(bytesRead, testData.length());
        CHECK_EQ(std::string(reinterpret_cast<char*>(readBuffer.data()), bytesRead), testData);
    }
    TearDown();
}

TEST_CASE_FIXTURE(GzipHandlerTest, "LargeDataTest") {
    SetUp();
    const size_t largeDataSize = 1024 * 1024; // 1 MB
    const size_t largeDataSizeInBytes = largeDataSize * sizeof(int);
    std::vector<int> largeData(largeDataSize);
    for (long long i = 0; i < largeDataSize; ++i) {
        largeData[i] = static_cast<int>(i % (19999999));
    }

    // Write large data
    {
        LocalFileHandler fileHandler(tmpFileName, 'w');
        GzipHandler gzipHandler(fileHandler);
        
        size_t bytesWritten = gzipHandler.write(reinterpret_cast<unsigned char*>(largeData.data()), largeDataSizeInBytes);
        gzipHandler.close();
        
        CHECK_EQ(bytesWritten, largeDataSizeInBytes);
    }

    // Read and verify large data
    {
        LocalFileHandler fileHandler(tmpFileName, 'r');
        GzipHandler gzipHandler(fileHandler);
        
        std::vector<unsigned char> readBuffer(largeDataSizeInBytes);
        size_t bytesRead = gzipHandler.read(readBuffer.data(), readBuffer.size());

        CHECK_EQ(bytesRead, largeDataSizeInBytes);
        for(int i = 0; i < largeDataSize; ++i) {
            int read_value = *reinterpret_cast<int*>(readBuffer.data() + i * sizeof(int));
            CHECK_EQ(read_value, largeData[i]);
        }
    }
    TearDown();
}

TEST_CASE_FIXTURE(GzipHandlerTest, "MultipleWritesTest") {
    SetUp();
    LocalFileHandler fileHandler(tmpFileName, 'w');
    GzipHandler gzipHandler(fileHandler);

    const std::string data1 = "First chunk of data. ";
    const std::string data2 = "Second chunk of data. ";
    const std::string data3 = "Third chunk of data.";

    gzipHandler.write(reinterpret_cast<unsigned char*>(const_cast<char*>(data1.c_str())), data1.length());
    gzipHandler.write(reinterpret_cast<unsigned char*>(const_cast<char*>(data2.c_str())), data2.length());
    gzipHandler.write(reinterpret_cast<unsigned char*>(const_cast<char*>(data3.c_str())), data3.length());
    gzipHandler.close(); 
    
    // Read and verify
    LocalFileHandler readFileHandler(tmpFileName, 'r');
    GzipHandler readGzipHandler(readFileHandler);

    std::vector<unsigned char> readBuffer(100);
    size_t totalBytesRead = 0;
    std::string result;

    while (true) {
        size_t bytesRead = readGzipHandler.read(readBuffer.data(), readBuffer.size());
        if (bytesRead == 0) break;
        result.append(reinterpret_cast<char*>(readBuffer.data()), bytesRead);
        totalBytesRead += bytesRead;
    }

    CHECK_EQ(result, data1 + data2 + data3);
    CHECK_EQ(totalBytesRead, data1.length() + data2.length() + data3.length());
    TearDown();
}
