#ifndef DQtH
#define DQtH

#ifdef QT_GUI_LIB

#include <QWidget>
#include <QLayout>
#include <QMainWindow>
#include <QString>
#include <QSettings>
#include <QStringList>
#include "libdpp/DPreferences.h"
#include "libdpp/DFilesystem.h"

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

namespace DApp
{
    bool SetStyleSheetFromFile(fs::path QssFilename);
}

namespace DShell
{
    void ShowInFolder(const QString& path);
    bool Execute(const QString& Filename, const QStringList& Args = QStringList(), int WaitMSecs = 30000, qint64 *Pid = nullptr);
    bool ExecuteDetached(const QString& Filename, const QStringList& Args = QStringList(),qint64 *Pid = nullptr);
}

namespace DWidget {
#include <QGridLayout>
#include <QWidget>

    /**
     * Utility class to remove the contents of a QGridLayout row, column or
     * cell. If the deleteWidgets parameter is true, then the widgets become
     * not only removed from the layout, but also deleted. Note that we won't
     * actually remove any row or column itself from the layout, as this isn't
     * possible. So the rowCount() and columnCount() will always stay the same,
     * but the contents of the row, column or cell will be removed.
     *
     * https://stackoverflow.com/questions/5395266/removing-widgets-from-qgridlayout
     */
    class QGridLayoutUtil {
        public:
            // Removes the contents of the given layout row.
            static void RemoveRow(QGridLayout *layout, int row, bool deleteWidgets = true);
            // Removes the contents of the given layout column.
            static void RemoveColumn(QGridLayout *layout, int column, bool deleteWidgets = true);
            // Removes the contents of the given layout cell.
            static void RemoveCell(QGridLayout *layout, int row, int column, bool deleteWidgets = true);
            // Removes all.
            static void RemoveAll(QGridLayout *layout, bool deleteWidgets = true);

        private:
            // Removes all layout items which span the given row and column.
            static void Remove(QGridLayout *layout, int row, int column, bool deleteWidgets);
            // Deletes all child widgets of the given layout item.
            static void DeleteChildWidgets(QLayoutItem *item);
    };

    class QTabWidgetUtil {
        public:
            static int IndexOf(QString TabName, QTabWidget *TabWidget);
    };
}
}
#endif
#endif
