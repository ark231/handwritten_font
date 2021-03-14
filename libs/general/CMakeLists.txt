cmake_minimum_required(VERSION 3.16)
project(handfont_general_libs CXX)
add_library(helpers SHARED helpers.cpp)
target_compile_features(parse_chardef PUBLIC cxx_std_17)
