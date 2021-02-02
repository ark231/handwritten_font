cmake_minimum_required(VERSION 3.16)
project(handfont_parse_chardef CXX)
add_library(parse_chardef SHARED parse_chardef.cpp)
target_compile_features(parse_chardef PUBLIC cxx_std_17)
