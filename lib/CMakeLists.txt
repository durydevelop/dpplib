set(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/DCsv.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DPath.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DPathWatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DPreferences.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DShell.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DString.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DStringGrid.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DSyncFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DSyncWatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DUpdate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DValue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DVector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/DNetwork.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/DRESTClient.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/network/DTcpServer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/DUdpServer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/DUri.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qt/DQt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qt/DFormLog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qt/DFormLog.ui
)

#add_library(${TARGET_NAME} ${TARGET_SRC_FILES})
#target_link_libraries(dpp)
#list(APPEND TARGETS ${TARGET_NAME})
#propagate sources to parent scope for one-lib-build
#set(${TARGET_NAME}_SRC_FILES ${TARGET_SRC_FILES} PARENT_SCOPE)
#set(TARGETS ${TARGETS} ${TARGET_NAME} PARENT_SCOPE)
set(SRC_FILES ${SRC_FILES} ${FILES} PARENT_SCOPE)

#install(
#  TARGETS lib-dpp
#  ARCHIVE DESTINATION lib
#  LIBRARY DESTINATION lib
#)
