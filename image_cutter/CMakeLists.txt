cmake_minimum_required(VERSION 3.16)
project(handfont_image_cutter CXX)
find_package(
    Boost 1.71
    COMPONENTS program_options
    REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(
    OpenCV 4.3
    COMPONENTS core imgcodecs imgproc objdetect
    REQUIRED)
pkg_check_modules(spdlog spdlog REQUIRED)

file(GLOB CPP_EXIFTOOL_SOURCE "${handfont_SOURCE_DIR}/cpp_exiftool/src/*.cpp")
add_library(read_image_metadata STATIC read_metadata.cpp image_utils.cpp ${CPP_EXIFTOOL_SOURCE})
target_include_directories(read_image_metadata PUBLIC ${handfont_SOURCE_DIR}/cpp_exiftool/inc)
target_link_libraries(read_image_metadata PUBLIC ${spdlog_LIBRARIES})
target_compile_features(read_image_metadata PUBLIC cxx_std_20)
target_compile_options(read_image_metadata PUBLIC $<IF:$<CONFIG:Debug>,-g3 -O0,>)
target_compile_options(read_image_metadata PUBLIC -Wall -Wextra)

add_executable(
    image_cutter
    image_cutter.cpp
    image_handler.cpp
    image_utils.cpp
    corner.cpp
    points.cpp
    qr_codes.cpp
    write_area.cpp
    sides.cpp)
target_include_directories(image_cutter PUBLIC ${Boost_INCLUDE_DIR})
target_include_directories(image_cutter PUBLIC ${handfont_libs_SOURCE_DIR})
target_include_directories(image_cutter PUBLIC ${OpenCV_INCLUDE_DIR})
target_link_libraries(image_cutter LINK_PUBLIC ${Boost_LIBRARIES})
target_link_libraries(image_cutter PUBLIC parse_chardef chardef_filemeta chardef_cvt_consts helpers read_image_metadata)
target_link_libraries(image_cutter PUBLIC ${OpenCV_LIBRARIES})
target_link_libraries(image_cutter PUBLIC ${spdlog_LIBRARIES})
target_link_libraries(image_cutter PUBLIC toml11)
target_compile_options(image_cutter PUBLIC $<IF:$<CONFIG:Debug>,-g3 -O0,>)
target_compile_options(image_cutter PUBLIC -Wall -Wextra)
target_compile_features(image_cutter PUBLIC cxx_std_20)
