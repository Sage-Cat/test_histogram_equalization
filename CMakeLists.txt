cmake_minimum_required(VERSION 2.8.12)
project(histo_equalis_test)

add_compile_options(-std=c++14)

include_directories(test_cpp)
set(SRC test_cpp/Image.cpp
        test_cpp/ImageProcessing.cpp
        test_cpp/Test.cpp
        test_cpp/Timer.cpp
        test_cpp/main.cpp
)

add_executable(histo_equalis_test ${SRC})
