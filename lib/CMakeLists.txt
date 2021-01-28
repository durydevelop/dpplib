set (SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/DCsv.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dlog.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DPath.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DPathWatcher.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DPreferences.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DShell.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DString.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DStringGrid.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DSyncFile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DSyncWatcher.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DUpdate.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DWindow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/network/DNetwork.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/network/DRestClient.cpp
	#${CMAKE_CURRENT_SOURCE_DIR}/network/DTcpServer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/network/DUdpServer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/network/DUri.cpp
	#${CMAKE_CURRENT_SOURCE_DIR}/qt/DQt.cpp
)

# Propagate sources to parent scope
set(SRC_FILES ${SRC_FILES} PARENT_SCOPE)
