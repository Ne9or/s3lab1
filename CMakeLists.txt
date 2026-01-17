cmake_minimum_required(VERSION 3.16)

project(SequenceProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Include)


add_executable(main
    src/main.cpp
    src/LazySequence.inl
)

target_include_directories(main
    PRIVATE
        ${PROJECT_INCLUDE_DIR}
)


include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests
    tests/LazySequenceTests.cpp
    src/LazySequence.inl
)

target_include_directories(tests
    PRIVATE
        ${PROJECT_INCLUDE_DIR}
)

target_link_libraries(tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)
