cmake_minimum_required(VERSION 3.16)
project(handfont_image_cutter CXX)
find_package(Boost 1.71 COMPONENTS program_options REQUIRED )
#find_package(ICU 61.0 COMPONENTS uc REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(OpenCV 4.3 COMPONENTS 
	core 
	imgcodecs 
	#highgui 
	imgproc 
	objdetect 
	REQUIRED)
pkg_check_modules(spdlog spdlog REQUIRED)
#find_package(Exiv2 REQUIRED)
#find_package(Magick++ REQUIRED)
#pkg_check_modules(Exiv2 exiv2 REQUIRED)
#pkg_check_modules(Magick++ Magick++ REQUIRED)
#pkg_check_modules(ImageMagick ImageMagick REQUIRED)
#message("inc ${Magick++_INCLUDEDIR}")
#message("inc ${Magick++_INCLUDE_DIRS}")
#message("inc ${ImageMagick_INCLUDEDIR}")
#message("inc ${Magick++_INCLUDES}")
#message("inc ${Magick++_ARCH_INCLUDEDIRS}")
#message("inc ${Magick++_ARCH_INCLUDE_DIR}")
#message("inc ${Magick++_ARCH_INCLUDE_DIRS}")
#message("lib ${Magick++_LIBRARIES}")
#message("def ${Magick++_DEFINITIONS}")

##for debugging
#message(STATUS "*** dump start cmake variables ***")
#get_cmake_property(_variableNames VARIABLES)
#foreach(_variableName ${_variableNames})
#	message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

file(GLOB CPP_EXIFTOOL_SOURCE "${handfont_SOURCE_DIR}/cpp_exiftool/src/*.cpp")
add_library(read_image_metadata STATIC
	read_metadata.cpp
	image_utils.cpp
	${CPP_EXIFTOOL_SOURCE}
	)
target_include_directories(read_image_metadata PUBLIC ${handfont_SOURCE_DIR}/cpp_exiftool/inc)
#target_include_directories(read_image_metadata PUBLIC ${Exiv2_INCLUDE_DIR})
#target_include_directories(read_image_metadata PUBLIC ${Magick++_INCLUDE_DIRS})
#target_link_libraries(read_image_metadata PUBLIC ${Exiv2_LIBRARIES})
#target_link_libraries(read_image_metadata PUBLIC ${Magick++_LIBRARIES})
target_link_libraries(read_image_metadata PUBLIC ${spdlog_LIBRARIES})
target_compile_features(read_image_metadata PUBLIC cxx_std_20)
target_compile_options(read_image_metadata PUBLIC $<IF:$<CONFIG:Debug>,-g3 -O0,>)
target_compile_options(read_image_metadata PUBLIC -Wall -Wextra )
#target_compile_options(read_image_metadata PUBLIC ${Magick++_CFLAGS})

add_executable(image_cutter 
	image_cutter.cpp 
	image_handler.cpp
	image_utils.cpp 
	corner.cpp 
	points.cpp 
	qr_codes.cpp 
	write_area.cpp
	sides.cpp
	)
target_include_directories(image_cutter PUBLIC ${Boost_INCLUDE_DIR} )
target_include_directories(image_cutter PUBLIC ${handfont_libs_SOURCE_DIR})
target_include_directories(image_cutter PUBLIC ${handfont_SOURCE_DIR}/toml11)
target_include_directories(image_cutter PUBLIC ${OpenCV_INCLUDE_DIR})
target_link_libraries(image_cutter LINK_PUBLIC ${Boost_LIBRARIES} )
target_link_libraries(image_cutter PUBLIC 
	parse_chardef 
	chardef_filemeta 
	chardef_cvt_consts 
	helpers
	read_image_metadata
	)
target_link_libraries(image_cutter PUBLIC ${OpenCV_LIBRARIES})
#target_link_libraries(image_cutter PUBLIC ${ICU_LIBRARIES})
target_link_libraries(image_cutter PUBLIC ${spdlog_LIBRARIES})
target_compile_options(image_cutter PUBLIC $<IF:$<CONFIG:Debug>,-g3 -O0,>)
target_compile_options(image_cutter PUBLIC -Wall -Wextra )
target_compile_features(image_cutter PUBLIC cxx_std_20)
