# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

include_directories(${PROJECT_SOURCE_DIR})
add_executable(rest-client rest-client.cpp ${SRC_FILES} ${HDR_FILES})

if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  target_link_libraries(rest-client ${PROJECT_NAME} ws2_32 wsock32 stdc++fs libdpp)
endif()

set_target_properties(rest-client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/example)
