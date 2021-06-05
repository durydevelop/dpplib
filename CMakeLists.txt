cmake_minimum_required(VERSION 3.11)

project(DPPLIB
        VERSION 0.2.1.4
        DESCRIPTION "DppLib is a Cpp extension library for boost, beast, asio"
        LANGUAGES CXX
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

######
# Options
option(${PROJECT_NAME}_BUILD_SHARED_LIBS "Build as shared library" OFF)
option(${PROJECT_NAME}_BUILD_INTERFACE_LIBS "Build as interface lib" OFF)
option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" OFF)
option(${PROJECT_NAME}_BUILD_TESTS "Build tests" OFF)
option(CMAKE_BUILD_TYPE "Debug" "Debug")
if (QT_FOUND)
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
    message_c(${BOLD_CYAN} "Build as: SHARED LIB")
    # Set SHARED ON
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS ON)
    set(BUILD_SHARED_LIBS ON)
    # Set INTERFACE OFF
    set(${PROJECT_NAME}_BUILD_INTERFACE_LIBS OFF)
    set(BUILD_INTERFACE_LIBS OFF)
elseif(${PROJECT_NAME}_BUILD_INTERFACE_LIBS OR BUILD_INTERFACE_LIBS)
    message_c(${BOLD_CYAN} "Build as: INTERFACE LIB")
    # Set INTERFACE ON
    set(${PROJECT_NAME}_BUILD_INTERFACE_LIBS ON)
    set(BUILD_INTERFACE_LIBS ON)
    # Set SHARED OFF
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF)
else()
    # STATIC (default)
    message_c(${BOLD_CYAN} "Build as: STATIC LIB")
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
else()
    set(${PROJECT_NAME}_BUILD_QT OFF)
    set(BUILD_QT OFF)
endif()

message_c(${BOLD_CYAN} "Build examples: ${BUILD_EXAMPLES}")
message_c(${BOLD_CYAN} "Build tests: ${BUILD_TESTS}")
message_c(${BOLD_CYAN} "Build Qt extension: ${BUILD_QT}")

#############################################################################
# Create Target
# CMakeFiles.txt in lib and include dirs set SRC_FILES with source files list
add_subdirectory(lib)
add_subdirectory(include)
if (${BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()

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
    target_link_libraries(${PROJECT_NAME} stdc++fs pthread)
    # winsock
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_link_libraries(${PROJECT_NAME} ws2_32 wsock32)
    endif()
endif()

# Find boost
include(DBoostFinder)

# Set make commad option Debug/Release
include(DMakeOptions)

# Download and install Google test library if needed
include(DGoogleTestHelper)

# Config Qt stuffs
if(${BUILD_QT})
    message_c("Project <${PROJECT_NAME}> is finding Qt libraries...")
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
    message_c("\tFound Qt${QT_VERSION_MAJOR}")
    if (NOT ${BUILD_INTERFACE_LIBS})
        target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Widgets)
    endif()
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
    message_c("Generate Install directive")
    include(DInstall)
endif()
