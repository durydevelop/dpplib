######################################################################################################################
# This CMake project file is based on this guide https://github.com/robotology/how-to-export-cpp-library/tree/master #
######################################################################################################################
cmake_minimum_required(VERSION 3.13)

#### WASM support
# CMAKE_TOOLCHAIN_FILE must be set before project() otherwise it does not take effect
if (${PLATFORM} MATCHES "Web" OR EMSCRIPTEN)
    if (NOT CMAKE_TOOLCHAIN_FILE)
        if (DEFINED ENV{EMSDK})
            set(CMAKE_TOOLCHAIN_FILE "$ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" CACHE STRING "" FORCE)
            #include(${CMAKE_TOOLCHAIN_FILE})
        else()
            message(FATAL_ERROR  "Emscripten non found, cannot compile on PLATFORM Web")
        endif()
    endif()
endif()

#### Create project
project(dpplib
        VERSION 0.5.0.0
        DESCRIPTION "dpplib is a Cpp library with a lot of helper class and tools"
        LANGUAGES C CXX
)

#### Common libraries options
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS yes)

#[[
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
]]#

#### CMake modules helper
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list (REMOVE_DUPLICATES CMAKE_MODULE_PATH)
include(DCMakeHelper)

#### Default options
option(${PROJECT_NAME}_BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" OFF)
# Top level options
if (PROJECT_IS_TOP_LEVEL)
    option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" ON)
    option(${PROJECT_NAME}_INSTALL "Create install target" ON)
else()
    option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" OFF)
    option(${PROJECT_NAME}_INSTALL "Create install target" OFF)
endif()
# Network
if (EMSCRIPTEN)
    option(${PROJECT_NAME}_BUILD_NET "Build networking support" OFF)
else()
    option(${PROJECT_NAME}_BUILD_NET "Build networking support" ON)
endif()
# Qt
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widget)
if (DEFINED QT_VERSION_MAJOR)
    # default ON if qt is already found
    option(${PROJECT_NAME}_BUILD_QT "Build Qt extension lib" ON)
else()
    # default OFF if qt is not found
    # set QT_DIR to force cmake find qt in
    option(${PROJECT_NAME}_BUILD_QT "Build Qt extension lib" OFF)
endif()
# Testing
option(${PROJECT_NAME}_BUILD_TESTING "Build tests suite" OFF)

#### Handle options (also set ${PROJECT_NAME}_<option> and <option> the same)
# Shared / static lib
if (${PROJECT_NAME}_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS ON)
    set(BUILD_SHARED_LIBS ON)
else()
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF)
endif()
# EMSCRIPTEN
if (${PROJECT_NAME}_EMSCRIPTEN OR EMSCRIPTEN)
    set(${PROJECT_NAME}_EMSCRIPTEN ON)
    set(EMSCRIPTEN ON)
else()
    set(${PROJECT_NAME}_EMSCRIPTEN OFF)
    set(EMSCRIPTEN OFF)
endif()
# Network
if (${PROJECT_NAME}_BUILD_NET OR BUILD_NET)
    set(${PROJECT_NAME}_BUILD_NET ON)
    set(BUILD_NET ON)
else()
    set(${PROJECT_NAME}_BUILD_NET OFF)
    set(BUILD_NET OFF)
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
if (${PROJECT_NAME}_BUILD_TESTING OR BUILD_TESTING)
    set(${PROJECT_NAME}_BUILD_TESTING ON)
    set(BUILD_TESTING ON)
else()
    set(${PROJECT_NAME}_BUILD_TESTING OFF)
    set(BUILD_TESTING OFF)
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

#### Encourage user to specify a build type (e.g. Release, Debug, etc.), otherwise set it to Debug.
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'Debug' as none was specified.")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Debug")
    endif()
endif()

#### Sources
# Source library files
set(${PROJECT_NAME}_SRC)
# Header library files
set(${PROJECT_NAME}_HDR)
# Include base dir
set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

## Add sources
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)
list(APPEND ${PROJECT_NAME}_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/TODO.md
)

#message("${PROJECT_NAME}_SRC=${${PROJECT_NAME}_SRC}")
#message("${PROJECT_NAME}_HDR=${${PROJECT_NAME}_HDR}")
#message("${PROJECT_NAME}_INCLUDE_DIRS=${${PROJECT_NAME}_INCLUDE_DIRS}")

#### Setup target
add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})

if (CMAKE_VERSION VERSION_LESS 3.28)
    # Use PUBLIC_HEADER
    target_sources(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_HDR})
    set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${${PROJECT_NAME}_HDR}")
else()
    # Use FILE_SET for HEADERS
    target_sources(${PROJECT_NAME}
        PUBLIC
            FILE_SET HEADERS
            BASE_DIRS include
            FILES ${${PROJECT_NAME}_HDR}
    )
endif()

# Alias
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Specify include directories for both compilation and installation process.
target_include_directories(${PROJECT_NAME}
    PUBLIC 
        "$<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIRS}>"
        "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
)

#### Install stuff
if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.28)
    if (${${PROJECT_NAME}_INSTALL})
        # Enable RPATH support for installed binaries and libraries
        include(AddInstallRPATHSupport)
        add_install_rpath_support(
            BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"
            LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"
            INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
            USE_LINK_PATH
        )

        # Specify installation targets, typology and destination folders.
        if (CMAKE_VERSION VERSION_LESS 3.28)
            # Using PUBLIC_HEADER
            install(TARGETS ${PROJECT_NAME}
                EXPORT  ${PROJECT_NAME}
                INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}        COMPONENT dev
                LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                     COMPONENT shlib
                ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                     COMPONENT lib
                RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"                     COMPONENT bin
                PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" COMPONENT dev
            )
            message(FATAL "Install is not supported, cannot create correct folder structure. You need to use cmake 3.28 or higher")
        else()
            # Using FILE_SET
            install(TARGETS ${PROJECT_NAME}
                    EXPORT  ${PROJECT_NAME}
                    FILE_SET HEADERS
                    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}        COMPONENT dev
                    LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                     COMPONENT shlib
                    ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                     COMPONENT lib
                    RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"                     COMPONENT bin
            )
        endif()

        # Create and install CMake configuration files
        include(InstallBasicPackageFiles)
        install_basic_package_files(${PROJECT_NAME}
            VERSION ${${PROJECT_NAME}_VERSION}
            COMPATIBILITY AnyNewerVersion
            VARS_PREFIX ${PROJECT_NAME}
            NO_CHECK_REQUIRED_COMPONENTS_MACRO
        )

        # Add the uninstall target
        include(AddUninstallTarget)
    endif()
else()
    set(${PROJECT_NAME}_INSTALL OFF)
    message_c(${BOLD_YELLOW} "You need cmake 2.28 or greater to set install targets. Now ${${PROJECT_NAME}_INSTALL} is set to OFF")
endif()

#### EMSCRIPTEN settings
if (EMSCRIPTEN)
	# Da provare ad attivare
	#set(USE_ZLIB OFF)
    
    # Flag for dpplib (Boost support)
    set(EMSDK_FLAGS "${EMSDK_FLAGS} -s USE_BOOST_HEADERS=1") # -s WARN_ON_UNDEFINED_SYMBOLS")

    # Add compile flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EMSDK_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSDK_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EMSDK_FLAGS}")

    # Includes
#    target_include_directories(${PROJECT_NAME} PUBLIC ${EMSCRIPTEN_SYSROOT}/include)
endif()

#### Dependencies
# Boost
if (NOT boost_FOUND)
    get_filename_component(Boost_ROOT "../boost" ABSOLUTE) # same as BOOST_ROOT_DIR "../../../../lib/boost"
    include(DBoostFinder)
endif()
# Winsock
if (${PROJECT_NAME}_BUILD_NET)
    # winsock
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        message_c("Linking Winsock2 for " ${PROJECT_NAME})
        target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32 wsock32)
    endif()
endif()


#### Integration tests
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

#### Examples
if (${PROJECT_NAME}_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

#### Doxygen support
if (PROJECT_IS_TOP_LEVEL)
    set(DOXYGEN_THEME_NAME example)
    set(DOXYGEN_LANGUAGE Italian)
    add_subdirectory(doxygen)
#    include(doxygen/Doxygen.cmake)
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
message_c(${BOLD_CYAN} "PROJECT_IS_TOP_LEVEL:     ${PROJECT_IS_TOP_LEVEL}")
message_c(${BOLD_CYAN} "BUILD_SHARED_LIBS:        ${${PROJECT_NAME}_BUILD_SHARED_LIBS}")
message_c(${BOLD_CYAN} "Build examples:           ${${PROJECT_NAME}_BUILD_EXAMPLES}")
message_c(${BOLD_CYAN} "Build testing:            ${${PROJECT_NAME}_BUILD_TESTING}")
message_c(${BOLD_CYAN} "Build Qt extension:       ${BUILD_QT}")
message_c(${BOLD_CYAN} "Build networking support: ${BUILD_NET}")
if (EMSCRIPTEN)
    message_c(${BOLD_CYAN} "WASM support:             ON (EMSCRIPTEN)")
    message_c(${BOLD_CYAN} "CMAKE_TOOLCHAIN_FILE:     ${CMAKE_TOOLCHAIN_FILE}")
endif()
