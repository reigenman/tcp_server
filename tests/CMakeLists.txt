find_package(GTest REQUIRED)


# Tests need to be added as executables first
add_executable(testlib test_simple_server.cpp)

target_link_libraries(testlib PRIVATE simple_server GTest::gtest GTest::gtest_main)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME testlibrary COMMAND testlib) # Command can be a target
