if ((NOT DEFINED ${Boost_FOUND}) OR (NOT ${Boost_FOUND}))
    message(${DSTATUS} "Project <${PROJECT_NAME}> is finding boost library...")
    if (NOT ${BOOST_ROOT} STREQUAL "")
        message(${DSTATUS} "BOOST_ROOT forced to " ${BOOST_ROOT})
    endif()
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        message(${DSTATUS} "\tfor Windows")
        if (${BOOST_ROOT})
            message(${DSTATUS} "\tFound BOOST_ROOT=" ${BOOST_ROOT})
        endif()
        find_package(Boost)
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        message(${DSTATUS} "\tFor Linux")
        find_package(Boost)
    endif()

    if (${Boost_FOUND})
        include_directories(${Boost_INCLUDE_DIR})
        message(${DSTATUS} "\tFound ${Boost_VERSION_STRING}")
        if (${Boost_VERSION_STRING} VERSION_LESS 1.66)
            message(FATAL_ERROR "This lib require Boost 1.66 or greater")
        endif()
        message(${DSTATUS} "\tBoost include dir: ${Boost_INCLUDE_DIR}")
        if (Boost_LIBRARY_DIR)
            message(${DSTATUS} "\tBoost lib: ${Boost_LIBRARY_DIR}")
            target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARY_DIR})
        endif()
    else()
        message(FATAL_ERROR ${DSTATUS} "Boost not found")
    endif()
endif()
