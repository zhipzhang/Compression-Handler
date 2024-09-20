# Find GTest package
find_package(GTest QUIET)

if(GTEST_FOUND)
    include(GoogleTest)

    # Set source files for tests
    set(TEST_SOURCES
        test_gzipfile.cpp
        test_xrootdfile.cpp
        test_localfile.cpp
        test_zstdfile.cpp
    )

    # Create the test executable
    add_executable(compression_tests ${TEST_SOURCES})

    # Include directories
    target_include_directories(compression_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${GTEST_INCLUDE_DIRS}
    )

    # Link libraries
    target_link_libraries(compression_tests PRIVATE
        compression_handlers
        GTest::Main
    )

    # Discover and add tests
    gtest_discover_tests(compression_tests)

    # Set properties for the target
    set_target_properties(compression_tests PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
else()
    message(STATUS "GTest not found. Test program will not be built.")
endif()
