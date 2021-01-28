cmake_minimum_required(VERSION 3.5)

project(rest-client LANGUAGES CXX)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

##############################################################
##                     Dury settings                        ##
##############################################################
message(">> CMake version: ${CMAKE_VERSION}")
message(">> gcc c++ version: ${CMAKE_CXX_COMPILER_VERSION}")

# set everything up for c++ 17 features
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Setup Boost lib
message(">> Finding boost library...")
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(">> for Windows")
    set(BOOST_ROOT "../../../../boost")
    #set(BOOST_INCLUDEDIR "../../../lib/cpp/boost")
    #set(BOOST_LIBRARY_DIR "../../../lib/cpp/boost/libs")
    find_package(Boost)

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(">> For Linux")
    find_package(Boost)
endif()

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    message(">> Found ${Boost_VERSION_STRING}")
    message(">> Includes: ${Boost_INCLUDE_DIR}")
    if (Boost_LIBRARY_DIR)
        message(">> Lib: ${Boost_LIBRARY_DIR}")
        #string(APPEND ADDED_LINK_LIBRARIES ${Boost_LIBRARY_DIR})
    endif()
else()
    message(FATAL_ERROR ">> boost not found")
endif()
# Remember to link stdc++fs:
# target_link_libraries(${PROJECT_NAME} PUBLIC stdc++fs)

##############################################################

add_executable(${PROJECT_NAME}
    main.cpp
    ../../DRESTClient.h
    ../../DRESTClient.cpp
    ../../DUri.h
    ../../DUri.cpp
)


## Link winsock
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32 wsock32)
endif()
