cmake_minimum_required(VERSION 3.16)
project(handfont_general_libs CXX)
add_library(helpers SHARED helpers.cpp)
target_compile_features(helpers PUBLIC cxx_std_17)
target_include_directories(helpers PUBLIC ${handfont_libs_SOURCE_DIR})
