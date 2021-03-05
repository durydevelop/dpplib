# Setup GoolgeTest library
if (BUILD_TESTS)
    message(">> Configuring GoogleTest library")
    enable_testing()
    include(FetchContent)
    set(FETCHCONTENT_QUIET OFF)
    set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
    set(FETCHCONTENT_BASE_DIR deps)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
    )
    FetchContent_MakeAvailable(googletest)

    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
    endif()
    add_subdirectory(tests)
endif()