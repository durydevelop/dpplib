#### Find googletest lib

function(get_package_from_git)
    include(FetchContent)
    set(FETCHCONTENT_QUIET OFF)
    set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
    set(FETCHCONTENT_BASE_DIR _deps)
    FetchContent_Declare(
        ${ARGV0}
        GIT_REPOSITORY ${ARGV1}
        GIT_TAG main
        #GIT_PROGRESS TRUE
    )
    # Check if population has already been performed
    FetchContent_GetProperties(${ARGV0})
    if(NOT ${ARGV0}_POPULATED)
        FetchContent_Populate(${ARGV0})
        add_subdirectory(${${ARGV0}_SOURCE_DIR} ${${ARGV0}_BINARY_DIR})
    endif()
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(${ARGV0})
endfunction()

if (USE_EXTERNAL_googletest)
    # Do not use internal googletest
    message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} is finding <${BOLD_CYAN}googletest${BOLD_MAGENTA}>")
    if (googletest_GIT_REPOSITORY)
        # GIT forced
        get_package_from_git(googletest ${googletest_GIT_REPOSITORY})
        if (googletest_FOUND)
            message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}googletest${BOLD_MAGENTA}> loaded custom repo")
        endif()
    elseif(googletest_ROOT)
        # Local FORCED
        add_subdirectory(${googletest_ROOT} _deps/googletest)
    else()
        # Try to find in system
        find_package(googletest QUIET CONFIG)
        if (googletest_FOUND)
            get_target_property(googletest_INCLUDE_DIRECTORIES googletest::googletest INTERFACE_INCLUDE_DIRECTORIES)
            message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}googletest${BOLD_MAGENTA}> found in system")
        else()
            # Load from official repo
            get_package_from_git(googletest https://github.com/google/googletest.git)
            set(googletest_FOUND TRUE)
            message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}googletest${BOLD_MAGENTA}> loaded from official repo")
        endif()
    endif()
else()
    # Use embedded one
    message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} use <${BOLD_CYAN}googletest${BOLD_MAGENTA}> embedded")
    add_subdirectory(external/googletest)
    install(
        DIRECTORY ${googletest_SOURCE_DIR}/googletest
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_TARGET_NAME}"
        FILES_MATCHING PATTERN "*.h"
    )
endif()

#get_target_property(googletest_SOURCE_DIR googletest SOURCE_DIR)
#get_target_property(googletest_INCLUDE_DIRECTORIES googletest INTERFACE_INCLUDE_DIRECTORIES)

#print_var(googletest)
#print_target_prop(googletest)
