include_directories( ${gtest_SOURCE_DIR}/include)

set(
  TESTS
  )

foreach (test ${TESTS})
  add_executable(${test} ${test}.cpp)
  add_dependencies(${test} network-uri gtest_main)
  target_link_libraries(${test} ${CMAKE_THREAD_LIBS_INIT} network-uri gtest_main)
  if (OPENSSL_FOUND)
    target_link_libraries(${test} ${OPENSSL_LIBRARIES})
  endif()
  set_target_properties(${test}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${Uri_BINARY_DIR}/tests)
  add_test(${test}
    ${libdpp_BINARY_DIR}/tests/${test})
endforeach (test)

file(GLOB URI_LISTS *.txt)
file(COPY ${URI_LISTS} DESTINATION ${libdpp_BINARY_DIR}/test)
