#ifndef DQWindowH
#define DQWindowH

#ifdef QT_GUI_LIB

#include <QLayout>
#include <QMainWindow>
#include <dpplib/DPreferences.h>
#include <QSettings>

namespace DTools
{
namespace DWindow
{
    void LayoutSetEnabled(QLayout *Layout, bool Enabled);
    void LayoutSetVisible(QLayout *Layout, bool Visible);
    bool SaveQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences *DestPrefs = nullptr);
    bool SaveQWindowPosition(QMainWindow& qMainWindow, QString WindowName, DTools::DPreferences *DestPrefs = nullptr);
    bool SaveQWindowPosition(QDialog& qDialog, DTools::DPreferences *DestPrefs = nullptr);
    bool SaveQWindowPosition(QDialog& qDialog, QString WindowName, DTools::DPreferences *DestPrefs = nullptr);
    QSettings::Status SaveQWindowGeometry(QMainWindow& qMainWindow, QString CompanyName = QString(), QString AppName = QString());
    QSettings::Status SaveQWindowGeometry(QDialog& qDialog, QString CompanyName = QString(), QString AppName = QString());
    bool RestoreQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences *DestPrefs = nullptr);
    bool RestoreQWindowPosition(QMainWindow& qMainWindow, QString WindowName, DTools::DPreferences *DestPrefs = nullptr);
    bool RestoreQWindowPosition(QDialog& qDialog, DTools::DPreferences *DestPrefs = nullptr);
    bool RestoreQWindowPosition(QDialog& qDialog, QString WindoName, DTools::DPreferences *DestPrefs = nullptr);
    QSettings::Status RestoreQWindowGeometry(QMainWindow& qMainWindow, QString CompanyName = QString(), QString AppName = QString());
    QSettings::Status RestoreQWindowGeometry(QDialog& qDialog, QString CompanyName = QString(), QString AppName = QString());
    void ShowInfoDialog(QString Title, QString Content, int Width, int Height);
}
}
#endif
#endif
