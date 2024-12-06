# Set System paths for ordinary linux systems
set(SYSTEM_LIBRARY_PATHS /usr/lib/x86_64-linux-gnu /usr/lib /usr/local/lib)

# Find required libraries
find_package(ZLIB REQUIRED)
if(NOT ZLIB_FOUND)
    message(FATAL_ERROR "ZLIB library not found. Please install ZLIB and try again.")
endif()
# Find ZSTD library
find_package(ZSTD QUIET)
if(NOT ZSTD_FOUND)
    message(WARNING "ZSTD library not found. Try to find it in the system paths.")
    find_library(ZSTD_LIBRARY NAMES zstd HINTS ${SYSTEM_LIBRARY_PATHS})
    if(ZSTD_LIBRARY)
        set(ZSTD_FOUND TRUE)
        set(ZSTD_LIBRARIES ${ZSTD_LIBRARY})
    endif()
    if(NOT ZSTD_FOUND)
        message(STATUS "ZSTD library not found. Continuing without ZSTD support.")
        return()
    endif()
endif()

find_package(XRootD COMPONENTS CLIENT QUIET)
if(NOT XROOTD_FOUND)
    message(WARNING "XRootD library not found. Try to find it in the system paths. For ihep platfrom, xrootd is placed in old_xrootd directory ")
    find_library(XrdCl_LIBRARY NAMES XrdCl HINTS  /cvmfs/lhaaso.ihep.ac.cn/anysw/slc5_ia64_gcc73/external/lib64/olds_xrootd)
    if(XrdCl_LIBRARY)
        set(XROOTD_FOUND TRUE)
        set(XROOTD_INCLUDE_DIRS /cvmfs/lhaaso.ihep.ac.cn/anysw/slc5_ia64_gcc73/external/include/xrootd)
    endif()
endif()

if(NOT XROOTD_FOUND)
    message(FATAL_ERROR "XRootD library not found. Please install XRootD or set XROOTD_DIR environment variable and try again.")
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
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

# Install the library
install(TARGETS compression_handlers
    LIBRARY DESTINATION lib64
    ARCHIVE DESTINATION lib64
    RUNTIME DESTINATION bin
)

# Install the header files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
