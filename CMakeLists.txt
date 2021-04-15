cmake_minimum_required(VERSION 3.11)

project(DPPLIB
        VERSION 0.2.1.2
        DESCRIPTION "DppLib is a Cpp extension library for boost, beast, asio"
        LANGUAGES CXX
)

######
# Options
set(BOOST_ROOT "" CACHE STRING "Boost root dir")
option(${PROJECT_NAME}_BUILD_SHARED_LIBS "Build as shared library" OFF)
option(${PROJECT_NAME}_BUILD_INTERFACE_LIBS "Build as interface lib" OFF)
option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" OFF)
option(${PROJECT_NAME}_BUILD_TESTS "Build tests" OFF)
option(CMAKE_BUILD_TYPE "Debug" "Debug")

######
# CMake helper scripts from git
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_BASE_DIR deps)
FetchContent_Declare(
    dcmake
    GIT_REPOSITORY git@gitlab.com:durydevelop/cpp/helpers_cmake.git
    #GIT_PROGRESS TRUE
)
# Check if population has already been performed
FetchContent_GetProperties(dcmake)
if(NOT helpers_cmake_POPULATED)
    message(${DSTATUS} "Not populated")
    FetchContent_Populate(dcmake)
endif()
FetchContent_MakeAvailable(dcmake)

# Now set CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${dcmake_SOURCE_DIR})
include(DMainHelper)

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

if (${BUILS_INTERFACE_LIBS})
    # Build as interfcae
    add_library(${PROJECT_NAME} INTERFACE)
else()
    # Build controlled by BUILD_SHARED_LIBS prop
    add_library(${PROJECT_NAME} ${SRC_FILES})
endif()

if (NOT ${BUILD_INTERFACE_LIBS})
    target_link_libraries(${PROJECT_NAME} stdc++fs pthread)
    # winsock
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_link_libraries(${PROJECT_NAME} ws2_32 wsock32)
    endif()
endif()

include(DBoostFinder)

# Install directives
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(${DSTATUS} "Generate Install directive")
    # generate install directive only if stand-alone project
    include(DInstall)
endif()

include(DMakeOptions)

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/lib>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>"

    #"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>"
)
#set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
#set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

include(DGoogleTestHelper)
