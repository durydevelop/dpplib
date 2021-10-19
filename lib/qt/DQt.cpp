#include "libdpp/qt/DQt.h"

#ifdef QT_GUI_LIB

#include <QLayout>
#include <QFileInfo>
#include <QDesktopServices>
#include <QScreen>
#include <QGuiApplication>
#include <QApplication>
#include <QUrl>
#include <QProcess>
#include <QDir>
#include <libdpp/DWindow.h>
#include <libdpp/DPath.h>

namespace DTools {
namespace DWindow {
    /**
     * @brief Enable/disable all widget inside a QLayout
     * @param Layout	->	Pointer to layout container
     * @param Enabled	->	Enable state
     */
    void LayoutSetEnabled(QLayout *Layout, bool Enabled) {
        for (int ixW=0; ixW<Layout->count(); ++ixW) {
            QWidget *Widget = Layout->itemAt(ixW)->widget();
            if(Widget != nullptr) Widget->setEnabled(Enabled);
        }
    }

    /**
     * @brief Show/hide all widget inside a QLayout
     * @param Layout	->	Pointer to layout container
     * @param Visible	->	Visible state
     */
    void LayoutSetVisible(QLayout *Layout, bool Visible) {
        for (int ixW=0; ixW<Layout->count(); ++ixW) {
            QWidget *Widget = Layout->itemAt(ixW)->widget();
            if(Widget != nullptr) Widget->setVisible(Visible);
        }
    }

    /**
     * @brief Save QWindow position data
     * @details Save position and size of a QWindow using DTools::DPreferences::DPreferences class
     * @param qWindow   ->  Reference to a QWindow object
     * @param DestPrefs ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created
     * @return true on success, false if cannot store data into prefs
     */
    bool SaveQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences *DestPrefs) {
        QWidget& qWidget=(QWidget&) qMainWindow;
        return(DTools::DWindow::SaveWindowPositionData(qWidget.objectName().toStdString(),qWidget.x(),qWidget.y(),qWidget.width(),qWidget.height(),DestPrefs));
    }

    /**
     * @brief Save QWindow position data
     * @details Save position and size of a QDialog using DTools::DPreferences::DPreferences class
     * @param qDialog   ->  Reference to a QDialog object.
     * @param DestPrefs ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created.
     * @return true on success, false if cannot store data into prefs.
     */
    bool SaveQWindowPosition(QDialog& qDialog, DTools::DPreferences *DestPrefs) {
        QWidget& qWidget=(QWidget&) qDialog;
        return(DTools::DWindow::SaveWindowPositionData(qWidget.objectName().toStdString(),qWidget.x(),qWidget.y(),qWidget.width(),qWidget.height(),DestPrefs));
    }

    /**
     * @brief Save Qt Window geometry data (pure Qt api version)
     * @details QMainWindow version: save geometry and state (menus, info bars, ...) information using QSettings api
     * @param qWindow       ->  Reference to a QMainWindow object.
     * @param CompanyName   ->  Used to store informations in correct way. If empty QSettings default rules are used.
     * @param AppName       ->  Used to store informations in correct way. If empty QSettings default rules are used.
     */
    QSettings::Status SaveQWindowGeometry(QMainWindow& qMainWindow, QString CompanyName, QString AppName) {
        QSettings qSettings(CompanyName,AppName);
        QSettings::Status Status=qSettings.status();
        if (Status == QSettings::NoError) {
            qSettings.setValue(qMainWindow.objectName()+"/Geometry",qMainWindow.saveGeometry());
            qSettings.setValue(qMainWindow.objectName()+"/State",qMainWindow.saveState());
        }
        return(Status);
    }

    /**
     * @brief Save Qt Window geometry (pure Qt api version)
     * @details Save QWindow geometry information using QSettings api
     * @param qDialog       ->  Reference to a QDialog object.
     * @param CompanyName   ->  Used to store informations in correct way. If empty QSettings default rules are used.
     * @param AppName       ->  Used to store informations in correct way. If empty QSettings default rules are used.
     */
    QSettings::Status SaveQWindowGeometry(QDialog& qDialog, QString CompanyName, QString AppName) {
        QSettings qSettings(CompanyName,AppName);
        QSettings::Status Status=qSettings.status();
        if (Status == QSettings::NoError) {
            QWidget& qWidget=(QWidget&) qDialog;
            qSettings.setValue(qWidget.objectName()+"/Geometry",qWidget.saveGeometry());
        }
        return(Status);
    }

    /**
     * @brief Restore Qt Window position data
     * @details Extract and set position and size of a QWindow using DTools::DPreferences::DPreferences class
     * @param qWindow   ->  Reference to a QWindow object
     * @param DestPrefs ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created
     * @return true on success, false if cannot store data into prefs
     */
     bool RestoreQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences *DestPrefs) {
        //return(RestoreQWindowPosition((QWindow&)qMainWindow,DestPrefs));
        int X,Y,Width,Height;
        if (!DTools::DWindow::RestoreWindowPositionData(qMainWindow.objectName().toStdString(),X,Y,Width,Height,DestPrefs)) {
                return false;
        }
        //if (X != 0 && Y != 0) {
        //    qMainWindow.move(X,Y);
        //}
        QScreen *screen = QGuiApplication::primaryScreen();

        if (Width > 0 && Height > 0) {
            if (Width > screen->geometry().width()) {
                Width=screen->geometry().width();
                X=0;
            }
            if (Height > screen->geometry().height()) {
                Height=screen->geometry().height();
                Y=0;
            }
            qMainWindow.resize(Width,Height);
        }

        qMainWindow.move(X,Y);

        return true;
    }

    /**
     * @brief Restore Qt Dialog position data.
     * @details Extract and set position and size of a QWindow using DTools::DPreferences::DPreferences class.
     * @param qDialog   ->  Reference to a QDialog object.
     * @param DestPrefs ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created.
     * @return true on success, false if cannot store data into prefs.
     */
    bool RestoreQWindowPosition(QDialog& qDialog, DTools::DPreferences *DestPrefs) {
        int X,Y,Width,Height;
        QWidget& qWidget=(QWidget&) qDialog;
        if (!DTools::DWindow::RestoreWindowPositionData(qWidget.objectName().toStdString(),X,Y,Width,Height,DestPrefs)) {
                return false;
        }
        if (X != 0 && Y != 0) {
            qWidget.move(X,Y);
        }
        if (Width > 0 && Height > 0) {
            qWidget.resize(Width,Height);
        }
        return true;
    }

    /**
     * @brief Restore Qt Main Window geometry (pure Qt api version)
     * @details QMainWindow version: restore geometry and state (menus, info bars, ...) using QSettings api
     * @param qWindow       ->  Reference to a QMainWindow object
     * @param CompanyName   ->  Used to store informations in correct way. If empty QSettings default rules are used
     * @param AppName       ->  Used to store informations in correct way. If empty QSettings default rules are used
     */
    QSettings::Status RestoreQWindowGeometry(QMainWindow& qMainWindow, QString CompanyName, QString AppName) {
        QSettings qSettings(CompanyName,AppName);
        QSettings::Status Status=qSettings.status();
        if (Status == QSettings::NoError) {
            qMainWindow.restoreGeometry(qSettings.value(qMainWindow.objectName()+"/Geometry").toByteArray());
            qMainWindow.restoreState(qSettings.value(qMainWindow.objectName()+"/State").toByteArray());
        }
        return(Status);
    }

    /**
     * @brief Restore Qt Dialog geometry (pure Qt api version)
     * @details Restore QDialog geometry information using QSettings api
     * @param qWindow       ->  Reference to a QMainWindow object
     * @param CompanyName   ->  Used to store informations in correct way. If empty QSettings default rules are used
     * @param AppName       ->  Used to store informations in correct way. If empty QSettings default rules are used
     */
    QSettings::Status RestoreQWindowGeometry(QDialog& qDialog, QString CompanyName, QString AppName) {
        QSettings qSettings(CompanyName,AppName);
        QSettings::Status Status=qSettings.status();
        if (Status == QSettings::NoError) {
            QWidget& qWidget=(QWidget&) qDialog;
            qWidget.restoreGeometry(qSettings.value(qWidget.objectName()+"/Geometry").toByteArray());
        }
        return(Status);
    }
}

namespace DApp {
    bool SetStyleSheetFromFile(fs::path QssFilename)
    {
        if (DPath::Exists(QssFilename)) {
            QFile QssFile(QssFilename.string().c_str());
            if (QssFile.open(QFile::ReadOnly)) {
                qApp->setStyleSheet(QString(QssFile.readAll()));
                //Log->d("Style Sheet "+Params->CurrStyleSheet+".qss caricato");
                return true;
            }
            else {
                //Log->e("Style Sheet "+Params->CurrStyleSheet+".qss ERRORE lettura");
                return false;
            }
        }
        else {
            //Log->e("Style Sheet "+Params->CurrStyleSheet+".qss NON ESISTE");
            return false;
        }
    }
}

namespace DShell
{
    void ShowInFolder(const QString& path) {
        QFileInfo info(path);
        #if defined _WIN32 || defined _WIN64
            QStringList args;
            if (!info.isDir())
                args << "/select,";
            args << QDir::toNativeSeparators(path);
            if (QProcess::startDetached("explorer", args))
                return;
        #elif defined(Q_OS_MAC)
            QStringList args;
            args << "-e";
            args << "tell application \"Finder\"";
            args << "-e";
            args << "activate";
            args << "-e";
            args << "select POSIX file \"" + path + "\"";
            args << "-e";
            args << "end tell";
            args << "-e";
            args << "return";
            if (!QProcess::startDetached("/usr/bin/osascript", args))
                return;
        #else
            QDesktopServices::openUrl(QUrl::fromLocalFile(info.isDir()? path : info.path()));
        #endif
    }

    bool Execute(const QString& Filename, const QStringList& Args, int WaitMSecs, qint64 *Pid) {
        QFileInfo info(Filename);
        if (info.isDir()) return false;
        QProcess Process;

        Process.start(Filename,Args);
        if (WaitMSecs != 0) {
            if (!Process.waitForStarted(WaitMSecs)) {
                return false;
            }
        }
        if (Pid) *Pid=Process.processId();

        if (WaitMSecs != 0) {
            return(Process.waitForFinished(WaitMSecs));
        }

        return true;
    }

    bool ExecuteDetached(const QString& Filename, const QStringList& Args, qint64 *Pid) {
        QFileInfo info(Filename);
        if (info.isDir()) return false;
        return(QProcess::startDetached(Filename,Args,"",Pid));
    }
}

namespace DWidget {
    // Removes the contents of the given layout row.
    void QGridLayoutUtil::RemoveRow(QGridLayout *layout, int row, bool deleteWidgets) {
        Remove(layout, row, -1, deleteWidgets);
        layout->setRowMinimumHeight(row, 0);
        layout->setRowStretch(row, 0);
    }

    // Removes the contents of the given layout column.
    void QGridLayoutUtil::RemoveColumn(QGridLayout *layout, int column, bool deleteWidgets) {
        Remove(layout, -1, column, deleteWidgets);
        layout->setColumnMinimumWidth(column, 0);
        layout->setColumnStretch(column, 0);
    }

    // Removes the contents of the given layout cell.
    void QGridLayoutUtil::RemoveCell(QGridLayout *layout, int row, int column, bool deleteWidgets) {
        Remove(layout, row, column, deleteWidgets);
    }

    // Removes all
    void QGridLayoutUtil::RemoveAll(QGridLayout *layout, bool deleteWidgets) {
        Remove(layout, -1, -1, deleteWidgets);
    }

    // Removes all layout items which span the given row and column.
    void QGridLayoutUtil::Remove(QGridLayout *layout, int row, int column, bool deleteWidgets) {
        // We avoid usage of QGridLayout::itemAtPosition() here to improve performance.
        for (int i = layout->count() - 1; i >= 0; i--) {
            int r, c, rs, cs;
            layout->getItemPosition(i, &r, &c, &rs, &cs);
            if ((row == -1 || (r <= row && r + rs > row)) &&
                (column == -1 || (c <= column && c + cs > column))) {
                // This layout item is subject to deletion.
                QLayoutItem *item = layout->takeAt(i);
                if (deleteWidgets) {
                    DeleteChildWidgets(item);
                }
                delete item;
            }
        }
    }

    // Deletes all child widgets of the given layout item.
    void QGridLayoutUtil::DeleteChildWidgets(QLayoutItem *item) {
        QLayout *layout = item->layout();
        if (layout) {
            // Process all child items recursively.
            int itemCount = layout->count();
            for (int i = 0; i < itemCount; i++) {
                DeleteChildWidgets(layout->itemAt(i));
            }
        }
        delete item->widget();
    }
}
}

#endif
