cmake_minimum_required(VERSION 3.16)
project(handfont_font_maker NONE)
add_custom_target(font_maker ALL "cp" 
	${CMAKE_CURRENT_SOURCE_DIR}/font_maker.py
	${CMAKE_CURRENT_SOURCE_DIR}/svg_utils.py 
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#if(EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/font_maker.py)
#	file(REMOVE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/font_maker.py)
#endif()
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/font_maker.py DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#message("font maker copied")
