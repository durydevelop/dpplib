include_directories(${${PROJECT_NAME}_SOURCE_DIR}/include)

set(TESTS
    dpath_test_dirs
    dpath_test_files
	dupdate_test
)

foreach(test ${TESTS})
	add_executable(${test} ${test}.cpp)
	add_dependencies(${test} DPPLIB gtest_main)
	target_link_libraries(${test} ${CMAKE_THREAD_LIBS_INIT} DPPLIB gtest_main)
	#if (OPENSSL_FOUND)
	#	target_link_libraries(${test} ${OPENSSL_LIBRARIES})
	#endif()
	#set_target_properties(${test} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${${PROJECT_NAME}_BINARY_DIR}/tests)
	add_test(${test} ${${PROJECT_NAME}_BINARY_DIR}/tests/${test})
endforeach()

install(
    DIRECTORY
        ${${PROJECT_NAME}_SOURCE_DIR}/tests/find
    DESTINATION
        ${${PROJECT_NAME}_BINARY_DIR}/tests
)

#file(COPY ${URI_LISTS} DESTINATION ${Uri_BINARY_DIR}/test)
