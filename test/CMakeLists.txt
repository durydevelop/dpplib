include_directories(${${PROJECT_NAME}_SOURCE_DIR}/include)

#include_directories(${PROJECT_SOURCE_DIR})
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(PROJECT_UNIT_TESTS_NAME ${PROJECT_NAME}_unit_tests)
#[[
# Gather the header and source files
#file(GLOB UNIT_TESTS_INC_FILES ${PROJECT_SOURCE_DIR}/test/*.h)
file(GLOB UNIT_TESTS_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)
add_definitions(-DBOOST_TEST_DYN_LINK)
add_executable(${PROJECT_UNIT_TESTS_NAME} ${UNIT_TESTS_SRC_FILES} ${SRC_FILES})
target_link_libraries(${PROJECT_UNIT_TESTS_NAME})
enable_testing()
add_test(DemoTest ${PROJECT_UNIT_TESTS_NAME})
]]#

set(TESTS
    dpath_test_dirs
    dpath_test_files
    #dupdate_test
    dstring_test
)

foreach(test_name ${TESTS})
        add_executable(${test_name} ${test_name}.cpp)
        add_dependencies(${test_name} DPPLIB gtest_main) # gmock_main)
        target_link_libraries(${test_name} ${CMAKE_THREAD_LIBS_INIT} DPPLIB gtest_main) # gmock_main)
	#if (OPENSSL_FOUND)
	#	target_link_libraries(${test} ${OPENSSL_LIBRARIES})
	#endif()
	add_test(${test_name} ${${PROJECT_NAME}_BINARY_DIR}/test/${test_name})
	set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${${PROJECT_NAME}_BINARY_DIR}/test)
	set_tests_properties(${test_name} PROPERTIES ENVIRONMENT "GTEST_COLOR=1")
	set(CMAKE_CTEST_ARGUMENTS "--verbose" PARENT_SCOPE)

endforeach()


install (
    DIRECTORY
        ${${PROJECT_NAME}_SOURCE_DIR}/test/find
    DESTINATION
        ${${PROJECT_NAME}_BINARY_DIR}/test
)

add_custom_target(copy_test_files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/find
            ${CMAKE_CURRENT_BINARY_DIR}/find
)



#CTEST_OUTPUT_ON_FAILURE=1 GTEST_COLOR=1
