###################################################
# make install use:
# find_package(dpplib)
# target_link_libraries(${PROJECT_NAME} dpplib)
###################################################

cmake_minimum_required(VERSION 3.13)

project(dpplib
        VERSION 0.2.1.4
        DESCRIPTION "dpplib is a Cpp library with a lot of helper class and tools"
        LANGUAGES CXX
)

## Initial setup
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS yes)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshorten-64-to-32")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # Build with Multiple Processes
    target_compile_options(${PROJECT_NAME} PRIVATE /MP)
endif()

## Cmake modules helper
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(DCMakeHelper)

## Default options
option(${PROJECT_NAME}_BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" OFF)
option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" OFF)
option(${PROJECT_NAME}_BUILD_TESTS "Build tests" OFF)
option(${PROJECT_NAME}_BUILD_NET "Build networking support" ON)
# Qt
if (DEFINED QT_VERSION_MAJOR)
    # default ON if qt is already found
    option(${PROJECT_NAME}_BUILD_QT "Build Qt extension lib" ON)
else()
    # default OFF if qt is not found
    # set QT_DIR to force cmake find qt in
    option(${PROJECT_NAME}_BUILD_QT "Build Qt extension lib" OFF)
endif()

## Handle options
# Shared / static lib
if (${PROJECT_NAME}_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS ON)
    set(BUILD_SHARED_LIBS ON)
else()
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF)
endif()
# Examples
if (${PROJECT_NAME}_BUILD_EXAMPLES OR BUILD_EXAMPLES)
    set(${PROJECT_NAME}_BUILD_EXAMPLES ON)
    set(BUILD_EXAMPLES ON)
else()
    set(${PROJECT_NAME}_BUILD_EXAMPLES OFF)
    set(BUILD_EXAMPLES OFF)
endif()
# Tests
if (${PROJECT_NAME}_BUILD_TESTS OR BUILD_TESTS)
    set(${PROJECT_NAME}_BUILD_TESTS ON)
    set(BUILD_TESTS ON)
else()
    set(${PROJECT_NAME}_BUILD_TESTES OFF)
    set(BUILD_TESTS OFF)
endif()
# Qt
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
# Network
if (${PROJECT_NAME}_BUILD_NET OR BUILD_NET)
    set(${PROJECT_NAME}_BUILD_NET ON)
    set(BUILD_NET ON)
else()
    set(${PROJECT_NAME}_BUILD_NET OFF)
    set(BUILD_NET OFF)
endif()

## Add sources
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)
list(APPEND ${PROJECT_NAME}_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/TODO.md
)
if (${BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()

## Setup target
if(BUILD_SHARED_LIBS)
    # Building shared library
    if(MSVC)
        # Workaround for some projects
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
    add_library( ${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_SOURCES}
        ${${PROJECT_NAME}_HEADERS}
    )
    # Set library version
    set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
else()
    # Static library
    add_library(${PROJECT_NAME}
        ${${PROJECT_NAME}_SOURCES}
        ${${PROJECT_NAME}_HEADERS}
    )
endif()
# Includes
set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
include(GNUInstallDirs)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIRS}/>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${${PROJECT_NAME}_HEADERS}")
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
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

## Install/uninstall directives
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/
)
include(GNUInstallDirs)
set(INCLUDE_INSTALL_DIR 
    ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    CACHE PATH "Location of header files"
)
set(SYSCONFIG_INSTALL_DIR
    ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}
    CACHE PATH "Location of configuration files"
)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_MODULE_PATH}/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    PATH_VARS INCLUDE_INSTALL_DIR SYSCONFIG_INSTALL_DIR
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
# todo: need .pc ?
configure_file(
    ${CMAKE_MODULE_PATH}/pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    @ONLY
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
install(EXPORT ${PROJECT_NAME}
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
if(NOT TARGET uninstall)
  configure_file(
    ${CMAKE_MODULE_PATH}/uninstall.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()


## C++ Pre-processor variables
# dpplib_VERSION
add_compile_definitions(${PROJECT_NAME}_VERSION="${PROJECT_VERSION}")
# DPPLIB_VERSION
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
add_compile_definitions(${PROJECT_NAME_UPPER}_VERSION="${PROJECT_VERSION}")

## Print summary
message_c(${BOLD_CYAN} "Summary:")
message_c(${BOLD_CYAN} "Project name:             ${PROJECT_NAME} - Version: ${PROJECT_VERSION}")
message_c(${BOLD_CYAN} "BUILD_SHARED_LIBS:        ${BUILD_SHARED_LIBS}")
message_c(${BOLD_CYAN} "Build examples:           ${BUILD_EXAMPLES}")
message_c(${BOLD_CYAN} "Build tests:              ${BUILD_TESTS}")
message_c(${BOLD_CYAN} "Build Qt extension:       ${BUILD_QT}")
message_c(${BOLD_CYAN} "Build networking support: ${BUILD_NET}")
