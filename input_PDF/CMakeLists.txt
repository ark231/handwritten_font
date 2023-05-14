cmake_minimum_required(VERSION 3.16)
project(handfont_input_pdf CXX)
find_package(
    Boost 1.71
    COMPONENTS program_options
    REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(QRencode libqrencode REQUIRED)
find_library(HPDF libhpdf.so)
find_package(
    ICU 61.0
    COMPONENTS uc
    REQUIRED)

add_executable(gen_input_pdf generator.cpp input_pdf_generator.cpp)
target_include_directories(gen_input_pdf PUBLIC ${Boost_INCLUDE_DIR})
# target_include_directories(gen_input_pdf PUBLIC ${handfont_chardef_libs_SOURCE_DIR})
# target_include_directories(gen_input_pdf PUBLIC ${handfont_general_libs_SOURCE_DIR})
target_include_directories(gen_input_pdf PUBLIC ${handfont_libs_SOURCE_DIR})
target_link_libraries(gen_input_pdf LINK_PUBLIC ${Boost_LIBRARIES})
target_link_libraries(gen_input_pdf PUBLIC parse_chardef chardef_filemeta chardef_cvt_consts helpers)
target_link_libraries(gen_input_pdf PUBLIC ${QRencode_LINK_LIBRARIES})
target_link_libraries(gen_input_pdf PUBLIC ${HPDF})
target_link_libraries(gen_input_pdf PUBLIC ${ICU_LIBRARIES})
target_link_libraries(gen_input_pdf PUBLIC toml11)
target_compile_options(gen_input_pdf PUBLIC $<IF:$<CONFIG:Debug>,-g3 -O0,>)
target_compile_options(gen_input_pdf PUBLIC -Wall -Wextra)
target_link_options(gen_input_pdf PUBLIC -rdynamic)

# for debugging message(STATUS "*** dump start cmake variables ***") get_cmake_property(_variableNames VARIABLES)
# foreach(_variableName ${_variableNames}) message(STATUS "${_variableName}=${${_variableName}}") endforeach()
# message(STATUS "SDL2_LIBRARIES=${SDL2_LIBRARIES}") message(STATUS "SDL_LIBRARIES=${SDL_LIBRARIES}") message(STATUS
# "SDL_LIBRARIES=${SDL_LIBRARIES}") message(STATUS "*** dump end ***")
