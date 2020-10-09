#ifndef DWindowH
#define DWindowH

#include "DPreferences.h"
#include <string>

#ifdef QT_GUI_LIB
    #include <QWindow>
    #include <QMainWindow>
    #include <QSettings>
#endif

namespace DTools
{
    namespace DWindow {
        bool SaveWindowPositionData(std::string Name, size_t X, size_t Y, size_t Width, size_t Height, DTools::DPreferences::DPreferences *DestPrefs = nullptr);
        bool RestoreWindowPositionData(std::string Name, int& X, int& Y, int& Width, int& Height, DTools::DPreferences::DPreferences *DestPrefs = nullptr);
        #ifdef QT_GUI_LIB
            bool SaveQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences::DPreferences *DestPrefs = nullptr);
            bool SaveQWindowPosition(QDialog& qDialog, DTools::DPreferences::DPreferences *DestPrefs = nullptr);
            QSettings::Status SaveQWindowGeometry(QMainWindow& qMainWindow, QString CompanyName = QString(), QString AppName = QString());
            QSettings::Status SaveQWindowGeometry(QDialog& qDialog, QString CompanyName = QString(), QString AppName = QString());

            bool RestoreQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences::DPreferences *DestPrefs = nullptr);
            bool RestoreQWindowPosition(QDialog& qDialog, DTools::DPreferences::DPreferences *DestPrefs = nullptr);
            QSettings::Status RestoreQWindowGeometry(QMainWindow& qMainWindow, QString CompanyName = QString(), QString AppName = QString());
            QSettings::Status RestoreQWindowGeometry(QDialog& qDialog, QString CompanyName = QString(), QString AppName = QString());
        #endif
    }
};

#endif
