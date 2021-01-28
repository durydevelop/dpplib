cmake_minimum_required(VERSION 3.8)

# Use this snippet *after* PROJECT(xxx):
  SET(CMAKE_INSTALL_PREFIX "E:/Programming/Lib/cpp/libdpp/install" CACHE PATH "cambiato" FORCE)

project(libdpp
        VERSION 0.2.0.2
        DESCRIPTION "Cpp extension library for boost, beast, asio"
        LANGUAGES CXX
)


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
    message(">> \tfor Windows")
    set(BOOST_ROOT "../../../lib/cpp/boost")
    if (BOOST_ROOT)
        message(">> \tFound BOOST_ROOT=" ${BOOST_ROOT})
    endif()
    find_package(Boost)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(">> \tFor Linux")
    find_package(Boost)
endif()

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    message(">> \tFound ${Boost_VERSION_STRING}")
    message(">> \tBoost include dir: ${Boost_INCLUDE_DIR}")
    if (Boost_LIBRARY_DIR)
        message(">> \tBoost lib: ${Boost_LIBRARY_DIR}")
        #string(APPEND ADDED_LINK_LIBRARIES ${Boost_LIBRARY_DIR})
    endif()
else()
    message(FATAL_ERROR ">> Boost not found")
endif()

add_subdirectory(include)
add_subdirectory(lib)
#add_subdirectory(example)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${HDR_FILES})

target_include_directories(${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  target_link_libraries(${PROJECT_NAME} ws2_32 wsock32 stdc++fs)
endif()

#set_target_properties(${PROJECT_NAME} PROPERTIES
#    CXX_STANDARD 11
#    CXX_STANDARD_REQUIRED YES
#    CXX_EXTENSIONS NO
#    )

include(cmake/install.cmake)

message("CMAKE_INSTALL_PREFIX=" ${CMAKE_INSTALL_PREFIX})
