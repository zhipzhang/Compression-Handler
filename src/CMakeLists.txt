# Find required libraries
find_package(ZLIB REQUIRED)
if(NOT ZLIB_FOUND)
    message(FATAL_ERROR "ZLIB library not found. Please install ZLIB and try again.")
endif()
# Find ZSTD library
find_library(ZSTD_LIBRARY
    NAMES zstd
    PATHS /lib/x86_64-linux-gnu
    NO_DEFAULT_PATH
)

if(NOT ZSTD_LIBRARY)
    message(FATAL_ERROR "ZSTD library not found. Please ensure libzstd is installed.")
endif()

# Set ZSTD_INCLUDE_DIRS and ZSTD_LIBRARIES
set(ZSTD_INCLUDE_DIRS "/usr/include")  # Assuming the default include path
set(ZSTD_LIBRARIES ${ZSTD_LIBRARY})

message(STATUS "Found ZSTD: ${ZSTD_LIBRARY}")
message(STATUS "ZSTD include directory: ${ZSTD_INCLUDE_DIRS}")

find_package(XRootD REQUIRED)
if(NOT XROOTD_FOUND)
    message(FATAL_ERROR "XRootD library not found. Please install XRootD and try again.")
endif()

# Set source files
set(SOURCES
    GzipHandler.cpp
    XRootDFileHandler.cpp
    ZstdHandler.cpp
    CompressionHandler.cpp
)

# Create the dynamic library
add_library(compression_handlers SHARED ${SOURCES})
message(STATUS "XRootD include directories: ${XROOTD_INCLUDE_DIRS}")

# Include directories
target_include_directories(compression_handlers PUBLIC
    ${ZLIB_INCLUDE_DIRS}
    ${ZSTD_INCLUDE_DIRS}
    ${XROOTD_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(compression_handlers PUBLIC
    ${ZLIB_LIBRARIES}
    ${ZSTD_LIBRARIES}
    ${XROOTD_LIBRARIES}
)

# Set properties for the target
set_target_properties(compression_handlers PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Install the library
install(TARGETS compression_handlers
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
