# Set System paths for ordinary linux systems
set(CMAKE_SYSTEM_LIBRARY_PATHS /usr/lib/x86_64-linux-gnu /usr/lib /usr/local/lib)

# Find required libraries
find_library(ZLIB_LIBRARY NAMES z HINTS ${SYSTEM_LIBRARY_PATHS})
if(NOT ZLIB_LIBRARY)
    message(FATAL_ERROR "ZLIB library not found. Please install ZLIB and try again.")
endif()

# Find ZSTD library
find_library(ZSTD_LIBRARY NAMES zstd HINTS ${SYSTEM_LIBRARY_PATHS})
if(NOT ZSTD_LIBRARY)
    message(FATAL_ERROR "ZSTD library not found. Please install ZSTD and try again.")
endif()

# Find XRootD library
find_package(XRootD COMPONENTS CLIENT POSIX QUIET)
if(NOT XROOTD_FOUND)
    message(WARNING "XRootD library not found. Try to find it in the system paths. For ihep platfrom, xrootd is placed in old_xrootd directory ")
endif()


# Set source files
set(SOURCES
    ZstdHandler.cpp
    GzipHandler.cpp
    CompressionHandler.cpp
)

# Create the dynamic library
add_library(CompressionHandler SHARED ${SOURCES})

# Include directories
target_include_directories(CompressionHandler PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(CompressionHandler PUBLIC
    ${XROOTD_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(CompressionHandler PUBLIC
    ${ZLIB_LIBRARY}
    ${ZSTD_LIBRARY}
    ${XROOTD_CLIENT_LIBRARIES}
    ${XROOTD_POSIX_LIBRARIES}
)

