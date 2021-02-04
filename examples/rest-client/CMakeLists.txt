include_directories(${PROJECT_SOURCE_DIR})
add_executable(rest-client rest-client.cpp)

target_link_libraries(rest-client ${PROJECT_NAME} DPPLIB stdc++fs)
if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  target_link_libraries(rest-client ${PROJECT_NAME} ws2_32 wsock32)
endif()

set_target_properties(rest-client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/runtime/examples/rest-client)
