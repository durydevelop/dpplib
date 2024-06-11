include_directories(${PROJECT_SOURCE_DIR})
add_executable(rest-client rest-client.cpp)

target_link_libraries(rest-client ${PROJECT_NAME} dpplib)
