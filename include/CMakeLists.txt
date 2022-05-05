set(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DChrono.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DCsv.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DError.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DFilesystem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DLog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DPath.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DPathWatcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DPreferences.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DShell.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DString.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DStringGrid.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DSyncFile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DSyncWatcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DTree.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DUpdate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DValue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DVector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/DWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/network/DNetwork.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/network/DRESTClient.h
    #${CMAKE_CURRENT_SOURCE_DIR}/libdpp/network/DTcpServer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/network/DUdpServer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/network/DUri.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/qt/DCheckBoxDelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/qt/DFormLog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/qt/DProgressDelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/qt/DQLabel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/qt/DQt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/qt/DSpinnerWidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/libdpp/qt/DSplashScreen.h
    #${CMAKE_CURRENT_SOURCE_DIR}/libdpp/qt/ui_dformlog.h
)

# Propagate sources to parent scope
set(SRC_FILES ${SRC_FILES} ${FILES} PARENT_SCOPE)
