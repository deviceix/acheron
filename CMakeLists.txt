# this file is a part of Acheron library which is under MIT license; see LICENSE for more info

cmake_minimum_required(VERSION 3.30)
project(acheron)

option(ACHERON_BUILD_TEST "Acheron Build Test" ON)
option(ACHERON_BUILD_BENCH "Acheron Build Bench" OFF)

message(STATUS "Acheron Build Test: ${ACHERON_BUILD_TEST}")
message(STATUS "Acheron Build Bench: ${ACHERON_BUILD_BENCH}")

set(CMAKE_CXX_STANDARD 23)

set(SOURCES
        lib/stub.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# tests
if (ACHERON_BUILD_TEST)
    find_package(GTest REQUIRED)

    enable_testing()
    include(GoogleTest)

    set(ACHERON_TEST ${PROJECT_NAME}-test)
    set(SANITIZER_FLAGS
            "-fsanitize=address,leak,undefined"
            "-g"
#           "-fsanitize=thread,undefined"
            "-fno-omit-frame-pointer"
            "-fno-sanitize-recover=all"
    )

    add_executable(${ACHERON_TEST}
            tests/atomic/atomic.cpp
            tests/atomic/atomic_ops.cpp
            tests/cstring/memops.cpp
            tests/cstring/strops.cpp
            tests/memory/allocator.cpp
            tests/list.cpp
            tests/string.cpp
            tests/unordered_map.cpp
            tests/vector.cpp
    )

    target_include_directories(${ACHERON_TEST} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_compile_options(${ACHERON_TEST} PRIVATE
            ${SANITIZER_FLAGS}
    )

    target_link_options(${ACHERON_TEST} PRIVATE
            ${SANITIZER_FLAGS}
    )

    target_link_libraries(${ACHERON_TEST} PRIVATE
            ${PROJECT_NAME}
            GTest::GTest
            GTest::Main
    )

    gtest_discover_tests(${ACHERON_TEST})
endif()

# benches

# install
