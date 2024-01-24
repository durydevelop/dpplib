cmake_minimum_required(VERSION 3.13)

project(dpptools
        VERSION 0.2.1.4
        DESCRIPTION "dpptools is a Cpp extension library for boost, beast, asio"
        LANGUAGES CXX
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find for qt exists
#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets)

######
# Options
option(${PROJECT_NAME}_BUILD_SHARED_LIBS "Build as shared library" OFF)
option(${PROJECT_NAME}_BUILD_INTERFACE_LIBS "Build as interface lib" OFF)
option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" OFF)
option(${PROJECT_NAME}_BUILD_TESTS "Build tests" OFF)
option(${PROJECT_NAME}_BUILD_NET "Build networking support" ON)
option(CMAKE_BUILD_TYPE "Debug" "Debug")
if (DEFINED QT_VERSION_MAJOR)
    # default ON if qt is already found
    option(${PROJECT_NAME}_BUILD_QT "Build Qt extension lib" ON)
else()
    # default OFF if qt is not found
    # set QT_DIR to force cmake find qt in
    option(${PROJECT_NAME}_BUILD_QT "Build Qt extension lib" OFF)
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # only if stand-alone project
    set(BOOST_ROOT "" CACHE STRING "Boost root dir")
endif()

#[[
######
# CMake helper scripts from git
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
set(FETCHCONTENT_BASE_DIR deps)
FetchContent_Declare(
    dcmake
    GIT_REPOSITORY git@gitlab.com:durydevelop/cpp/helpers_cmake.git
    #GIT_PROGRESS TRUE
)
# Check if population has already been performed
FetchContent_GetProperties(dcmake)
if(NOT helpers_cmake_POPULATED)
    FetchContent_Populate(dcmake)
endif()
FetchContent_MakeAvailable(dcmake)
#]]
## Temporaneo per testare le modifiche
get_filename_component(dcmake_SOURCE_DIR "../../helpers_cmake" ABSOLUTE)

# Now set CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH ${dcmake_SOURCE_DIR})
list (REMOVE_DUPLICATES CMAKE_MODULE_PATH)
include(DMainHelper)

# Handle STATIC/SHARED/INTERFACE build option
if (${PROJECT_NAME}_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    # BUILD_SHARED_LIBS has prececence
    set(LIB_TYPE "SHARED")
    #message_c(${BOLD_CYAN} "Build as: SHARED LIB")
    # Set SHARED ON
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS ON)
    set(BUILD_SHARED_LIBS ON)
    # Set INTERFACE OFF
    set(${PROJECT_NAME}_BUILD_INTERFACE_LIBS OFF)
    set(BUILD_INTERFACE_LIBS OFF)
elseif(${PROJECT_NAME}_BUILD_INTERFACE_LIBS OR BUILD_INTERFACE_LIBS)
    set(LIB_TYPE "INTERFACE")
    #message_c(${BOLD_CYAN} "Build as: INTERFACE LIB")
    # Set INTERFACE ON
    set(${PROJECT_NAME}_BUILD_INTERFACE_LIBS ON)
    set(BUILD_INTERFACE_LIBS ON)
    # Set SHARED OFF
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF)
else()
    # STATIC (default)
    set(LIB_TYPE "STATIC")
    #message_c(${BOLD_CYAN} "Build as: STATIC LIB")
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

# Handle building tests option
if (${PROJECT_NAME}_BUILD_TESTS OR BUILD_TESTS)
    set(${PROJECT_NAME}_BUILD_TESTS ON)
    set(BUILD_TESTS ON)
else()
    set(${PROJECT_NAME}_BUILD_TESTES OFF)
    set(BUILD_TESTS OFF)
endif()

# Handle building Qt extension lib
if (${PROJECT_NAME}_BUILD_QT OR BUILD_QT)
    set(${PROJECT_NAME}_BUILD_QT ON)
    set(BUILD_QT ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
else()
    set(${PROJECT_NAME}_BUILD_QT OFF)
    set(BUILD_QT OFF)
endif()

# Handle building network support
if (${PROJECT_NAME}_BUILD_NET OR BUILD_NET)
    set(${PROJECT_NAME}_BUILD_NET ON)
    set(BUILD_NET ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
else()
    set(${PROJECT_NAME}_BUILD_NET OFF)
    set(BUILD_NET OFF)
endif()

#############################################################################
# Create Target
# CMakeFiles.txt in ./lib and ./include dirs set SRC_FILES with source files list
add_subdirectory(lib)
add_subdirectory(include)

if (${BUILS_INTERFACE_LIBS})
    # Build as interfcae
    add_library(${PROJECT_NAME} INTERFACE)
else()
    # Build controlled by BUILD_SHARED_LIBS prop
    add_library(${PROJECT_NAME}
        ${SRC_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        ${CMAKE_CURRENT_SOURCE_DIR}/TODO.md
    )
endif()

if (NOT ${BUILD_INTERFACE_LIBS})
    message_c("Linking filesystem lib for " ${PROJECT_NAME})
    target_link_libraries(${PROJECT_NAME} stdc++fs pthread)
    # winsock
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        message_c("Linking Winsock2 for " ${PROJECT_NAME})
        target_link_libraries(${PROJECT_NAME} ws2_32 wsock32)
    endif()
endif()

#set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC TRUE)

## Find boost
# Uncomment this line to force BOOST_ROOT folder
#get_filename_component(BOOST_ROOT "../boost" ABSOLUTE)
#include(DBoostFinder)
#set(Boost_INCLUDE_DIR ${Boost_INCLUDE_DIR} PARENT_SCOPE)

# Set make commad option Debug/Release
include(DMakeOptions)

# Config Qt stuffs
if(${BUILD_QT})
    message_c("Project <${PROJECT_NAME}> is configuring Qt libraries...")
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
    message_c("\tFound Qt${QT_VERSION_MAJOR}")
    if (NOT ${BUILD_INTERFACE_LIBS})
        target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Widgets)
    endif()
endif()

if (${BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()

# Download and install Google test library if needed
if (BUILD_TESTS)
    include(DGoogleTestHelper)
    add_subdirectory(test)
endif()

# Includes
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

# Install directives
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # generate install directive only if stand-alone project
    message_c("Generate ${PROJECT_NAME} install directive")
    # set Config.cmake.in source path
    set(CONFIG_CMAKE_IN_PATH ${dcmake_SOURCE_DIR})
    include(DInstall)
    message_c("Done")
endif()

message_c(${BOLD_CYAN} "Summary:")
message_c(${BOLD_CYAN} "Build as: ${LIB_TYPE} LIB")
message_c(${BOLD_CYAN} "Build examples: ${BUILD_EXAMPLES}")
message_c(${BOLD_CYAN} "Build tests: ${BUILD_TESTS}")
message_c(${BOLD_CYAN} "Build Qt extension: ${BUILD_QT}")
message_c(${BOLD_CYAN} "Build networking support: ${BUILD_NET}")
