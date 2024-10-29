# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
add_compile_definitions(BAKELITE_LAPTOP_MAC=$ENV{BAKELITE_LAPTOP_MAC})
add_compile_definitions(BAKELITE_PHONE_MAC=$ENV{BAKELITE_PHONE_MAC})

cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ptt-phone)
