#ifdef QT_GUI_LIB

#include <QWidget>
#include <QLayout>
#include <QMainWindow>
#include <QString>
#include <QSettings>
#include "libdpp/DPreferences.h"

namespace DTools
{
namespace DWindow
{
    void LayoutSetEnabled(QLayout *Layout, bool Enabled);
    void LayoutSetVisible(QLayout *Layout, bool Visible);
    bool SaveQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences *DestPrefs = nullptr);
    bool SaveQWindowPosition(QDialog& qDialog, DTools::DPreferences *DestPrefs = nullptr);
    QSettings::Status SaveQWindowGeometry(QMainWindow& qMainWindow, QString CompanyName = QString(), QString AppName = QString());
    QSettings::Status SaveQWindowGeometry(QDialog& qDialog, QString CompanyName = QString(), QString AppName = QString());
    bool RestoreQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences *DestPrefs = nullptr);
    bool RestoreQWindowPosition(QDialog& qDialog, DTools::DPreferences *DestPrefs = nullptr);
    QSettings::Status RestoreQWindowGeometry(QMainWindow& qMainWindow, QString CompanyName = QString(), QString AppName = QString());
    QSettings::Status RestoreQWindowGeometry(QDialog& qDialog, QString CompanyName = QString(), QString AppName = QString());
}

namespace DShell
{
    void ShowInFolder(const QString& path);
}
}
#endif
