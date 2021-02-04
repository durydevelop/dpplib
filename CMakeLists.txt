cmake_minimum_required(VERSION 3.11)

project(DPPLIB
        VERSION 0.2.1.2
        DESCRIPTION "DppLib is a Cpp extension library for boost, beast, asio"
        LANGUAGES CXX
)

######
# TODO
# STATIC SHARE INTERFACE modes
#

######
# User overrides
set(BOOST_ROOT "E:/Programming/Lib/cpp/boost")
option(BUILD_EXAMPLES "Build examples" OFF)

######
# Initial stuffs
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Stand-alone project
    set(DSTATUS ">> ")
else()
    # Part of other project
    set(DSTATUS ">>>> ")
    message(${DSTATUS} "Library ${PROJECT_NAME} included as external project <<<<")
endif()

message(${DSTATUS} "CMake version: ${CMAKE_VERSION}")
message(${DSTATUS} "gcc c++ version: ${CMAKE_CXX_COMPILER_VERSION}")
# Helpers for debug print
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DPrintHelpers)

# Set everything up for c++ 17 features
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

######
# Create Target
add_subdirectory(lib)
add_subdirectory(include)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
message(${DSTATUS} "Build examples: " ${BUILD_EXAMPLES})

# lib and include CMakeFiles set a variable SRC_FILES that contains source files list
add_library(${PROJECT_NAME} ${SRC_FILES})


if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # generate install directive only if stand-alone project
    include(Install)
endif()

#print_target_prop(DPPLIB)
#print_var("TARGET")
#print_target_ver(DPPLIB)

######
# Properties of targets

# Add definitions for targets
# Values:
#   * Debug: -D${PROJECT_NAME}_DEBUG=1
#   * Release: -D${PROJECT_NAME}_DEBUG=0
#   * other: -D${PROJECT_NAME}_DEBUG=0
target_compile_definitions(${PROJECT_NAME} PUBLIC "${PROJECT_NAME}_DEBUG=$<CONFIG:Debug>")

######
# Find Boost
if (NOT Boost_FOUND)
    message(${PROJECT_NAME} "-" ${DSTATUS} "Finding boost library...")
    if (DEFINED BOOST_ROOT)
        message(${DSTATUS} "BOOST_ROOT forced to " ${BOOST_ROOT})
    endif()
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        message(${DSTATUS} "\tfor Windows")
        if (BOOST_ROOT)
            message(${DSTATUS} "\tFound BOOST_ROOT=" ${BOOST_ROOT})
        endif()
        find_package(Boost)
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        message(${DSTATUS} "\tFor Linux")
        find_package(Boost)
    endif()

    if (Boost_FOUND)
        #include_directories(${Boost_INCLUDE_DIR})
        message(${DSTATUS} "\tFound ${Boost_VERSION_STRING}")
        if (${Boost_VERSION_STRING} VERSION_LESS 1.66)
            message(FATAL_ERROR "This lib requre Boost 1.66 or greater")
        endif()
        message(${DSTATUS} "\tBoost include dir: ${Boost_INCLUDE_DIR}")
        if (Boost_LIBRARY_DIR)
            message(${DSTATUS} "\tBoost lib: ${Boost_LIBRARY_DIR}")
        endif()
    else()
        message(FATAL_ERROR ${DSTATUS} "Boost not found")
    endif()
endif()

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/lib>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>"
)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

#[[
## Includes
include_directories(${${PROJECT_NAME}_SOURCE_DIR}/src ${${PROJECT_NAME}_SOURCE_DIR}/include)
#add_subdirectory(example)

install(DIRECTORY include DESTINATION ".")

#if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
#  target_link_libraries(${PROJECT_NAME} ws2_32 wsock32 stdc++fs)
#endif()

# Testing
if (BUILD_TESTS)
  message(">> Configuring tests")
  enable_testing()
  #find_package(GTest REQUIRED)
  #include_directories(${GTEST_INCLUDE_DIR})
  #add_subdirectory(deps/googletest)
  #add_subdirectory(test)
  # FetchContent added in CMake 3.11, downloads during the configure step
  # FetchContent_MakeAvailable was not added until CMake 3.14
  include(FetchContent)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    )
  FetchContent_MakeAvailable(googletest)

  FetchContent_GetProperties(googletest)
  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
  endif()
endif()

#include(cmake/install.cmake)
message(">>>> CMAKE_CURRENT_SOURCE_DIR " ${CMAKE_CURRENT_SOURCE_DIR})
message(">>>> CMAKE_SOURCE_DIR " ${CMAKE_SOURCE_DIR})

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(INCLUDE_INSTALL_DIR include/)
set(LIBRARY_INSTALL_DIR lib/)
set(SYSCONFIG_INSTALL_DIR etc/${PROJECT_NAME}/)

message(">>>> INCLUDE_INSTALL_DIR " ${INCLUDE_INSTALL_DIR})
message(">>>> LIBRARY_INSTALL_DIR " ${LIBRARY_INSTALL_DIR})
message(">>>> SYSCONFIG_INSTALL_DIR " ${SYSCONFIG_INSTALL_DIR})

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION
        cmake
    PATH_VARS
        INCLUDE_INSTALL_DIR
        SYSCONFIG_INSTALL_DIR
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION
        ${PROJECT_VERSION}
    COMPATIBILITY
        SameMajorVersion
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION
        cmake
)
#]]
