#ifndef DWindowH
#define DWindowH

#include "DPreferences.h"
#include <string>

#ifdef QT_GUI_LIB
    #include <QWindow>
    #include <QMainWindow>
#endif

namespace DTools
{
    namespace DWindow {
        bool SaveWindowPosition(std::string Name, size_t X, size_t Y, size_t Width, size_t Height, DTools::DPreferences::DPreferences *DestPrefs = nullptr);
        bool RestoreWindowPosition(std::string Name, int& X, int& Y, int& Width, int& Height, DTools::DPreferences::DPreferences *DestPrefs = nullptr);
        #ifdef QT_GUI_LIB
            bool SaveQWindowPosition(QWindow& qWindow, DTools::DPreferences::DPreferences *DestPrefs = nullptr);
            bool RestoreQWindowPosition(QWindow& qWindow, DTools::DPreferences::DPreferences *DestPrefs = nullptr);
            void SaveQWindowGeometry(QMainWindow& qWindow, QString CompanyName = QString(), QString AppName = QString());
            void RestoreQWindowGeometry(QMainWindow& qWindow, QString CompanyName = QString(), QString AppName = QString());
            void SaveQWindowGeometry(QWidget& qWindow, QString CompanyName = QString(), QString AppName = QString());
            void RestoreQWindowGeometry(QWidget& qWindow, QString CompanyName = QString(), QString AppName = QString());
        #endif
    }
};

#endif
