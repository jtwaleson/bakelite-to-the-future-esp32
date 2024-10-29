# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly

if(DEFINED ENV{BAKELITE_LAPTOP_MAC})
    set(BAKELITE_LAPTOP_MAC_ENV $ENV{BAKELITE_LAPTOP_MAC})
else()
    set(BAKELITE_LAPTOP_MAC_ENV "")
endif()

if(DEFINED ENV{BAKELITE_PHONE_MAC})
    set(BAKELITE_PHONE_MAC_ENV $ENV{BAKELITE_PHONE_MAC})
else()
    set(BAKELITE_PHONE_MAC_ENV "")
endif()

# Pass the environment variables to KconfiBAKELITE_LAPTOP_MACg
set(CONFIG_BAKELITE_LAPTOP_MAC "${BAKELITE_LAPTOP_MAC_ENV}" CACHE STRING "" FORCE)
set(CONFIG_BAKELITE_PHONE_MAC "${BAKELITE_PHONE_MAC_ENV}" CACHE STRING "" FORCE)

cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ptt-phone)
