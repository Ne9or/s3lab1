cmake_minimum_required(VERSION 3.16)

project(lazy_stream_counter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Исполняемый файл
add_executable(main
    src/main.cpp
)

# Пути к заголовкам
target_include_directories(main
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Предупреждения (по желанию, но полезно)
if (MSVC)
    target_compile_options(main PRIVATE /W4)
else()
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
endif()
