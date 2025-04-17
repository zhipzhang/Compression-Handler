    # Set source files for tests
    set(TEST_SOURCES
        test_localfile.cpp
        test_zstdfile.cpp
        test_gzipfile.cpp
    )

    # Create individual test executables for each source file
    foreach(test_source ${TEST_SOURCES})
        # Get the filename without extension
        get_filename_component(test_name ${test_source} NAME_WE)
        
        # Create executable for this test
        add_executable(${test_name} ${test_source})
        
        # Include directories
        target_include_directories(${test_name} PRIVATE
            ${CMAKE_SOURCE_DIR}/include
        )
        
        # Link libraries
        target_link_libraries(${test_name} PRIVATE
		CompressionHandler
        )
        
        # Set properties for the target
        set_target_properties(${test_name} PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
        )
        
        # Add as a test
        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
