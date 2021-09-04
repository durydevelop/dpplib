cmake_minimum_required(VERSION 3.11)

project(MegaDEMO
        VERSION 1.0.0.0
        LANGUAGES CXX
)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets QuickWidgets REQUIRED)
find_package(Quick)

######
# User overrides
set(${PROJECT_NAME}_COMPANY_NAME "Photorec S.r.l.")
set(${PROJECT_NAME}_COMPANY_COPYRIGHT "2020 Fabio Durigon")

######
# Initial stuffs
set(DSTATUS ">> ")
message(${DSTATUS} "CMake version: ${CMAKE_VERSION}")
message(${DSTATUS} "gcc c++ version: ${CMAKE_CXX_COMPILER_VERSION}")

# set everything up for c++ 17 features
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable console popup
if(WIN32)
    set(GUI_TYPE WIN32)
    message(${DSTATUS} "Set WIN32 gui")
elseif(APPLE)
    set(GUI_TYPE MACOSX_BUNDLE)
    message(${DSTATUS} "Set APPLE gui")
endif()

add_executable(MegaDEMO
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

######
# DPPLIB
add_subdirectory(../.. dep)
include_directories(../../include)
target_link_libraries(${PROJECT_NAME} PUBLIC DPPLIB)

######
## Link libraries
# Qt
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::QuickWidgets
)

target_link_libraries(MegaDEMO PUBLIC pthread stdc++fs)

# winsock
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32 wsock32)
endif()
