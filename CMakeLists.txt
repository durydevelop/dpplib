cmake_minimum_required(VERSION 3.11)

project(DPPLIB
        VERSION 0.2.1.2
        DESCRIPTION "DppLib is a Cpp extension library for boost, beast, asio"
        LANGUAGES CXX
)

######
# TODO
# STATIC OK
# SHARE OK
# INTERFACE
# TESTS
#
# Helpers for debug print
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DPrintHelpers)

######
# Options
set(BOOST_ROOT "E:/Programming/Lib/cpp/boost")
option(${PROJECT_NAME}_BUILD_SHARED_LIBS "Build as shared library" OFF)
option(${PROJECT_NAME}_BUILD_INTERFACE_LIBS "Build as interface lib" OFF)
option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" OFF)
option(${PROJECT_NAME}_BUILD_TESTS "Build tests" OFF)

######
# Init stuffs
# Set everything up for c++ 17 features
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set DSTATUS prefix for messages
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Stand-alone project
    set(DSTATUS ">> ")
else()
    # Part of other project
    set(DSTATUS ">>>> ")
    message(${DSTATUS} "Library ${PROJECT_NAME} included as external project <<<<")
endif()

# Start messages
message(${DSTATUS} "CMake version: ${CMAKE_VERSION}")
message(${DSTATUS} "gcc c++ version: ${CMAKE_CXX_COMPILER_VERSION}")

# Handle STATIC/SHARED/INTERFACE build option
if (${PROJECT_NAME}_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    # BUILD_SHARED_LIBS has prececence
    message(${DSTATUS} "Build as: SHARED LIB")
    # Set SHARED ON
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS ON)
    set(BUILD_SHARED_LIBS ON)
    # Set INTERFACE OFF
    set(${PROJECT_NAME}_BUILD_INTERFACE_LIBS OFF)
    set(BUILD_INTERFACE_LIBS OFF)
elseif(${PROJECT_NAME}_BUILD_INTERFACE_LIBS OR BUILD_INTERFACE_LIBS)
    message(${DSTATUS} "Build as: INTERFACE LIB")
    # Set INTERFACE ON
    set(${PROJECT_NAME}_BUILD_INTERFACE_LIBS ON)
    set(BUILD_INTERFACE_LIBS ON)
    # Set SHARED OFF
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF)
else()
    # STATIC
    message(${DSTATUS} "Build as: STATIC LIB")
    # Set SHARED OFF
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF)
    # Set INTERFACE OFF
    set(${PROJECT_NAME}_BUILD_INTERFACE_LIBS OFF)
    set(BUILD_INTERFACE_LIBS OFF)
endif()

# Handle builing examples option
if (${PROJECT_NAME}_BUILD_EXAMPLES OR BUILD_EXAMPLES)
    set(${PROJECT_NAME}_BUILD_EXAMPLES ON)
    set(BUILD_EXAMPLES ON)
else()
    set(${PROJECT_NAME}_BUILD_EXAMPLES OFF)
    set(BUILD_EXAMPLES OFF)
endif()

# Handle builing tests option
if (${PROJECT_NAME}_BUILD_TESTS OR BUILD_TESTS)
    set(${PROJECT_NAME}_BUILD_TESTS ON)
    set(BUILD_TESTS ON)
else()
    set(${PROJECT_NAME}_BUILD_TESTES OFF)
    set(BUILD_TESTS OFF)
endif()

message(${DSTATUS} "Build examples: " ${BUILD_EXAMPLES})
message(${DSTATUS} "Build tests: " ${BUILD_TESTS})

###################################################################
# Create Target
# CMakeFiles.txt in lib and include dirs set SRC_FILES with source files list
add_subdirectory(lib)
add_subdirectory(include)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

if (${BUILS_INTERFACE_LIBS})
    # Build as interfcae
    add_library(${PROJECT_NAME} INTERFACE)
else()
    # Build controlled bu BUILD_SHARED_LIBS prop
    add_library(${PROJECT_NAME} ${SRC_FILES})
endif()

if (NOT ${BUILD_INTERFACE_LIBS})
    target_link_libraries(${PROJECT_NAME} stdc++fs pthread)
    # winsock
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_link_libraries(${PROJECT_NAME} ws2_32 wsock32)
    endif()
endif()

# Install
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # generate install directive only if stand-alone project
    include(Install)
endif()

######
# Debug/Release config
ADD_CUSTOM_TARGET(debug
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Creating the executable in the debug mode.")

ADD_CUSTOM_TARGET(release
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Creating the executable in the release mode.")

######
# Find Boost
if (NOT Boost_FOUND)
    message(${DSTATUS} "Finding boost library...")
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

# Setup GoolgeTest library
if (BUILD_TESTS)
  message(">> Configuring GoogleTest")
  enable_testing()
  #find_package(GTest REQUIRED)
  #include_directories(${GTEST_INCLUDE_DIR})
  #add_subdirectory(deps/googletest)
  #add_subdirectory(test)
  # FetchContent added in CMake 3.11, downloads during the configure step
  # FetchContent_MakeAvailable was not added until CMake 3.14
  include(FetchContent)
  set(FETCHCONTENT_QUIET OFF)
  set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
  set(FETCHCONTENT_BASE_DIR deps)
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
