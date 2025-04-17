cmake_minimum_required(VERSION 3.14)
project(CompressionHandler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable warning flags
if(MSVC)
    add_compile_options(/W4)
else()
    #add_compile_options(-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_REENTRANT)
endif()

if(WITH_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
add_subdirectory(src)

include(GNUInstallDirs)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CompressionHandler)

install(TARGETS CompressionHandler
    EXPORT CompressionHandler-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT CompressionHandler-targets
    FILE CompressionHandlerTargets.cmake
    NAMESPACE CompressionHandler::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CompressionHandler
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/CompressionHandlerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CompressionHandlerConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CompressionHandler
)

# Install the config file
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/CompressionHandlerConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/CompressionHandler
)

