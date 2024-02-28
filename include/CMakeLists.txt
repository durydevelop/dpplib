# Main lib
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libdpp)

# Qt support
if (${BUILD_QT})
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libdpp/qt)
	
endif()

# Network support
if (${BUILD_NET})
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libdpp/network)
endif()

# Propagate sources to parent scope
set(SRC_FILES ${SRC_FILES} ${FILES} PARENT_SCOPE)
