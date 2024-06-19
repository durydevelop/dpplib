include_directories(${${PROJECT_NAME}_SOURCE_DIR}/include)


set(TESTS
    dpath_test_dirs
    dpath_test_files
    #dupdate_test
    dstring_test
)

foreach(test_name ${TESTS})
        add_executable(${test_name} ${test_name}.cpp)
        add_dependencies(${test_name} ${PROJECT_NAME} gtest_main) # gmock_main)
        target_link_libraries(${test_name} ${CMAKE_THREAD_LIBS_INIT} ${PROJECT_NAME} gtest_main) # gmock_main)
	#if (OPENSSL_FOUND)
	#	target_link_libraries(${test} ${OPENSSL_LIBRARIES})
	#endif()
	add_test(${test_name} ${${PROJECT_NAME}_BINARY_DIR}/test/${test_name})
	set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${${PROJECT_NAME}_BINARY_DIR}/test)
	set_tests_properties(${test_name} PROPERTIES ENVIRONMENT "GTEST_COLOR=1")
	set(CMAKE_CTEST_ARGUMENTS "--verbose" PARENT_SCOPE)

endforeach()

#[[
install (
    DIRECTORY
        ${${PROJECT_NAME}_SOURCE_DIR}/test/find
    DESTINATION
        ${${PROJECT_NAME}_BINARY_DIR}/test
)

install (
    DIRECTORY
        ${${PROJECT_NAME}_SOURCE_DIR}/test/move
    DESTINATION
        ${${PROJECT_NAME}_BINARY_DIR}/test
)
#]]
add_custom_target(copy_test_files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/find
            ${CMAKE_CURRENT_BINARY_DIR}/find
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/move
            ${CMAKE_CURRENT_BINARY_DIR}/move
)



#CTEST_OUTPUT_ON_FAILURE=1 GTEST_COLOR=1
