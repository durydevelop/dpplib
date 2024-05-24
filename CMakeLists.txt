cmake_minimum_required(VERSION 3.13)

project(dpplib
        VERSION 0.2.1.4
        DESCRIPTION "dpplib is a Cpp library with a lot of helper class and tools"
        LANGUAGES CXX
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find for qt exists
#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets)

######
# Options
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

# Now set CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
#list (REMOVE_DUPLICATES CMAKE_MODULE_PATH)
include(DMainHelper)

# Handle building examples option
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
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets)
else()
    set(${PROJECT_NAME}_BUILD_QT OFF)
    set(BUILD_QT OFF)
endif()

# Handle building network support
if (${PROJECT_NAME}_BUILD_NET OR BUILD_NET)
    set(${PROJECT_NAME}_BUILD_NET ON)
    set(BUILD_NET ON)
    #set(CMAKE_AUTOUIC ON)
    #set(CMAKE_AUTOMOC ON)
else()
    set(${PROJECT_NAME}_BUILD_NET OFF)
    set(BUILD_NET OFF)
endif()

#############################################################################
# Setup Target
# CMakeFiles.txt in ./lib and ./include dirs set SRC_FILES with source files list
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)
message_c("dpplib CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")
add_library(${PROJECT_NAME} INTERFACE
    ${SRC_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/TODO.md
)

## Setup Target
# Handle STATIC/SHARED/INTERFACE build option (default SHARED)
#set(${PROJECT_NAME}_LIBRARY_TYPE SHARED)
#include(DLibraryType)
#[[
if (${PROJECT_NAME}_BUILD_INTERFACE_LIBS)
    # Build as interface
    message_c("Target as INTERFACE")
    add_library(${PROJECT_NAME} INTERFACE)
elseif(${PROJECT_NAME}_BUILD_SHARED_LIBS)
    # Building shared library
    message_c("Target as SHARED")
    if(MSVC)
        # Workaround for some projects
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
    add_library(${PROJECT_NAME} SHARED
        ${SRC_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        ${CMAKE_CURRENT_SOURCE_DIR}/TODO.md
    )
else()
    # Static library
    message_c("Target as STATIC")
    add_library(${PROJECT_NAME} ${SRC_FILES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIRS})
endif()
]]#

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
#include(DMakeOptions)

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
    ${PROJECT_NAME} INTERFACE
#    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/lib>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
#    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
#    "$<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>"

#    "$<INSTALL_INTERFACE:include>"
)


#set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
#set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
#[[
# Install directives
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # generate install directive only if stand-alone project
    message_c("Generate ${PROJECT_NAME} install directive")
    # set Config.cmake.in source path
    #set(CONFIG_CMAKE_IN_PATH ${dcmake_SOURCE_DIR})
    include(DInstall)
    message_c("Done")
endif()
]]#
message_c(${BOLD_CYAN} "Summary:")
message_c(${BOLD_CYAN} "Project name:             ${PROJECT_NAME} - Version: ${PROJECT_VERSION}")
message_c(${BOLD_CYAN} "Build as:                 SHARED LIB")
message_c(${BOLD_CYAN} "Build examples:           ${BUILD_EXAMPLES}")
message_c(${BOLD_CYAN} "Build tests:              ${BUILD_TESTS}")
message_c(${BOLD_CYAN} "Build Qt extension:       ${BUILD_QT}")
message_c(${BOLD_CYAN} "Build networking support: ${BUILD_NET}")
