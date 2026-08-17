cmake_minimum_required(VERSION 3.20)
project(histogram_equalization VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_library(histogram_core
    test_cpp/Image.cpp
    test_cpp/ImageProcessing.cpp
    test_cpp/Test.cpp
)
target_include_directories(histogram_core PUBLIC test_cpp)

add_executable(histogram_equalize
    test_cpp/Timer.cpp
    test_cpp/main.cpp
)
target_link_libraries(histogram_equalize PRIVATE histogram_core)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(histogram_core PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(histogram_equalize PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(histogram_core PRIVATE /W4)
    target_compile_options(histogram_equalize PRIVATE /W4)
endif()

if(BUILD_TESTING)
    add_executable(histogram_tests tests/HistogramTests.cpp)
    target_link_libraries(histogram_tests PRIVATE histogram_core)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(histogram_tests PRIVATE -Wall -Wextra -Wpedantic)
    elseif(MSVC)
        target_compile_options(histogram_tests PRIVATE /W4)
    endif()
    add_test(NAME histogram_tests COMMAND histogram_tests)
endif()
