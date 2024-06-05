# Main lib
set(HDR
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DChrono.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DCsv.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DError.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DFilesystem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DLog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DPath.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DPathWatcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DPreferences.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DSettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DShell.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DString.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DStringGrid.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DSyncFile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DSyncWatcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DTree.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DUpdate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DValue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DVector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/DWindow.h
)

# Qt support
if (${BUILD_QT})
    set(HDR ${HDR}
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/qt/DQApp.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/qt/DQCheckBoxDelegate.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/qt/DFormLog.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/qt/DQProgressBar.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/qt/DQProgressBarDelegate.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/qt/DQLabel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/qt/DQProcess.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/qt/DQShell.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/qt/DQWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/qt/DQWindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/qt/DQSpinnerWidgetDelegate.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/qt/DQSpinnerWidget.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/qt/DSplashScreen.h
    )
endif()

# Network support
if (${BUILD_NET})
    set(HDR ${HDR}
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/network/DNetwork.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/network/DRESTClient.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/network/DUdpServer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/dpplib/network/DUri.h
    )
endif()

set(${PROJECT_NAME}_HDR ${${PROJECT_NAME}_HDR} ${HDR} PARENT_SCOPE)