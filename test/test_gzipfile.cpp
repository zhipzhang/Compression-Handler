#include <gtest/gtest.h>
#include <cstring>
#include <string>
#include "LocalFileHandler.h"
#include "GzipHandler.h"

class LocalCompressGzipTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary file for testing
        tmpFileName = "test_gzip_file.gz";
    }

    void TearDown() override {
        // Remove the temporary file
        //std::remove(tmpFileName.c_str());
    }

    std::string tmpFileName;
};

TEST_F(LocalCompressGzipTest, WriteAndReadTest) {
    const std::string testData = "Hello, Gzip compression!";
    
    // Write test
    {
        LocalFileHandler fileHandler(tmpFileName, 'w');
        GzipHandler gzipHandler(fileHandler);
        
        unsigned char* writeBuffer = reinterpret_cast<unsigned char*>(const_cast<char*>(testData.c_str()));
        size_t bytesWritten = gzipHandler.write(writeBuffer, testData.length());
        printf("bytesWritten: %zu\n", bytesWritten);
        EXPECT_EQ(bytesWritten, testData.length());
        gzipHandler.close();
    }

    // Read test
    {
        LocalFileHandler fileHandler(tmpFileName, 'r');
        GzipHandler gzipHandler(fileHandler);
        
        std::vector<unsigned char> readBuffer(100, 0);
        size_t bytesRead = gzipHandler.read(readBuffer.data(), readBuffer.size());

        EXPECT_EQ(bytesRead, testData.length());
        EXPECT_EQ(std::string(reinterpret_cast<char*>(readBuffer.data()), bytesRead), testData);
    }
}

TEST_F(LocalCompressGzipTest, LargeDataTest) {
    const size_t largeDataSize = 1024 * 1024; // 1 MB
    std::vector<unsigned char> largeData(largeDataSize);
    for (size_t i = 0; i < largeDataSize; ++i) {
        largeData[i] = static_cast<unsigned char>(i % 256);
    }

    // Write large data
    {
        LocalFileHandler fileHandler(tmpFileName, 'w');
        GzipHandler gzipHandler(fileHandler);
        
        size_t bytesWritten = gzipHandler.write(largeData.data(), largeData.size());
        gzipHandler.close();
        printf("bytesWritten: %zu\n", bytesWritten);
        EXPECT_EQ(bytesWritten, largeData.size());
    }

    // Read and verify large data
    {
        LocalFileHandler fileHandler(tmpFileName, 'r');
        GzipHandler gzipHandler(fileHandler);
        
        std::vector<unsigned char> readBuffer(largeDataSize);
        size_t bytesRead = gzipHandler.read(readBuffer.data(), readBuffer.size());

        EXPECT_EQ(bytesRead, largeData.size());
        EXPECT_EQ(readBuffer, largeData);
    }
}

TEST_F(LocalCompressGzipTest, MultipleWritesTest) {
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

    EXPECT_EQ(result, data1 + data2 + data3);
    EXPECT_EQ(totalBytesRead, data1.length() + data2.length() + data3.length());
}
