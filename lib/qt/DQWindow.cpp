#include <libdpp/qt/DQWindow.h>

#ifdef QT_GUI_LIB

#include <libdpp/DWindow.h>
#include <QDialog>
#include <QGuiApplication>
#include <QLabel>
#include <QPlainTextEdit>
#include <QScreen>

namespace DTools
{
namespace DWindow
{
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
     * @brief Save QMainWindow position data
     * @details Save position and size of a QMainWindow using DTools::DPreferences::DPreferences class
     * @param qWindow   ->  Reference to a QMainWindow object
     * @param DestPrefs ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created
     * @return true on success, false if cannot store data into prefs
     */
    bool SaveQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences *DestPrefs) {
        QWidget& qWidget=(QWidget&) qMainWindow;
        return(DTools::DWindow::SaveWindowPositionData(qWidget.objectName().toStdString(),qWidget.x(),qWidget.y(),qWidget.width(),qWidget.height(),DestPrefs));
    }

    /**
     * @brief Save QMainWindow position data using custom window name.
     * @details Save position and size of a QMainWindow using DTools::DPreferences::DPreferences class.
     * @param qWindow       ->  Reference to a QMainWindow object.
     * @param WindowName    ->  Name used to save tree in preferences.
     * @param DestPrefs     ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created.
     * @return true on success, false if cannot store data into prefs.
     */
    bool SaveQWindowPosition(QMainWindow& qMainWindow, QString WindowName, DTools::DPreferences *DestPrefs) {
        QWidget& qWidget=(QWidget&) qMainWindow;
        return(DTools::DWindow::SaveWindowPositionData(WindowName.toStdString(),qWidget.x(),qWidget.y(),qWidget.width(),qWidget.height(),DestPrefs));
    }

    /**
     * @brief Save QDialog position data.
     * @details Save position and size of a QDialog using DTools::DPreferences::DPreferences class.
     * @param qDialog   ->  Reference to a QDialog object.
     * @param DestPrefs ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created.
     * @return true on success, false if cannot store data into prefs.
     */
    bool SaveQWindowPosition(QDialog& qDialog, DTools::DPreferences *DestPrefs) {
        QWidget& qWidget=(QWidget&) qDialog;
        return(DTools::DWindow::SaveWindowPositionData(qWidget.objectName().toStdString(),qWidget.x(),qWidget.y(),qWidget.width(),qWidget.height(),DestPrefs));
    }

    /**
     * @brief Save QDialog position data using custom window name.
     * @details Save position and size of a QDialog using DTools::DPreferences::DPreferences class.
     * @param qDialog       ->  Reference to a QDialog object.
     * @param WindowName    ->  Name used to save tree in preferences.
     * @param DestPrefs     ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created.
     * @return true on success, false if cannot store data into prefs.
     */
    bool SaveQWindowPosition(QDialog& qDialog, QString WindowName, DTools::DPreferences *DestPrefs) {
        QWidget& qWidget=(QWidget&) qDialog;
        return(DTools::DWindow::SaveWindowPositionData(WindowName.toStdString(),qWidget.x(),qWidget.y(),qWidget.width(),qWidget.height(),DestPrefs));
    }

    /**
     * @brief Save Qt Window geometry data in QSettings (pure Qt api version).
     * @details QMainWindow version: save geometry and state (menus, info bars, ...) information using QSettings api.
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
     * @brief Save Qt Window geometry in QSettings (pure Qt api version).
     * @details Save QWindow geometry information using QSettings api.
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
     * @brief Restore QMainWindow position data.
     * @details Extract and set position and size of a QMainWindow using DTools::DPreferences::DPreferences class.
     * @param qWindow   ->  Reference to a QMainWindow object.
     * @param DestPrefs ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created.
     * @return true on success, false if cannot store data into prefs.
     */
     bool RestoreQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences *DestPrefs) {
        int X,Y,Width,Height;
        if (!DTools::DWindow::RestoreWindowPositionData(qMainWindow.objectName().toStdString(),X,Y,Width,Height,DestPrefs)) {
                return false;
        }

        QRect ScreenRect = QGuiApplication::primaryScreen()->availableGeometry();

        if (Width > 0 && Height > 0) {
            if (Width > ScreenRect.width()) {
                Width=ScreenRect.width();
                X=0;
            }
            if (Height > ScreenRect.height()) {
                Height=ScreenRect.height();
                Y=0;
            }
            qMainWindow.resize(Width,Height);
        }

        qMainWindow.move(X,Y);

        return true;
    }

     /**
      * @brief Restore QMainWindow position data using custom window name.
      * @details Extract and set position and size of a QMainWindow using DTools::DPreferences::DPreferences class.
      * @param qWindow      ->  Reference to a QMainWindow object.
      * @param WindowName   ->  Name used to save tree in preferences.
      * @param DestPrefs    ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created.
      * @return true on success, false if cannot store data into prefs.
      */
      bool RestoreQWindowPosition(QMainWindow& qMainWindow, QString WindowName, DTools::DPreferences *DestPrefs) {
         int X,Y,Width,Height;
         if (!DTools::DWindow::RestoreWindowPositionData(WindowName.toStdString(),X,Y,Width,Height,DestPrefs)) {
                 return false;
         }

         QRect ScreenRect = QGuiApplication::primaryScreen()->availableGeometry();
         qMainWindow.hide();

         if (Width > 0 && Height > 0) {
             if (Width > ScreenRect.width()) {
                 Width=ScreenRect.width();
                 X=0;
             }
             if (Height > ScreenRect.height()) {
                 Height=ScreenRect.height();
                 Y=0;
             }
             qMainWindow.resize(Width,Height);
         }

         qMainWindow.move(X,Y);
         qMainWindow.show();

         return true;
     }

    /**
     * @brief Restore QDialog position data.
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
     * @brief Restore QDialog position data using custom window name.
     * @details Extract and set position and size of a QWindow using DTools::DPreferences::DPreferences class.
     * @param qDialog   ->  Reference to a QDialog object.
     * @param WindowName   ->  Name used to save tree in preferences.
     * @param DestPrefs ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created.
     * @return true on success, false if cannot store data into prefs.
     */
    bool RestoreQWindowPosition(QDialog& qDialog, QString WindowName, DTools::DPreferences *DestPrefs) {
        int X,Y,Width,Height;
        QWidget& qWidget=(QWidget&) qDialog;
        if (!DTools::DWindow::RestoreWindowPositionData(WindowName.toStdString(),X,Y,Width,Height,DestPrefs)) {
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
     * @brief Restore Qt Main Window geometry from QSettings (pure Qt api version).
     * @details QMainWindow version: restore geometry and state (menus, info bars, ...) using QSettings api.
     * @param qWindow       ->  Reference to a QMainWindow object.
     * @param CompanyName   ->  Used to store informations in correct way. If empty QSettings default rules are used.
     * @param AppName       ->  Used to store informations in correct way. If empty QSettings default rules are used.
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
     * @brief Restore Qt Dialog geometry from QSettings (pure Qt api version).
     * @details Restore QDialog geometry information using QSettings api.
     * @param qWindow       ->  Reference to a QMainWindow object.
     * @param CompanyName   ->  Used to store informations in correct way. If empty QSettings default rules are used.
     * @param AppName       ->  Used to store informations in correct way. If empty QSettings default rules are used.
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

    void ShowInfoDialog(QString Title, QString Content, int Width, int Height) {
        // Dialog
        QDialog *dialog = new QDialog();
        if (Width > 10 && Height > 10) {
            dialog->resize(Height,Width);
        }
        // Layout
        QGridLayout *DialogLayout=new QGridLayout();
        // Tile label
        QLabel *Label=new QLabel(Title);
        Label->setAlignment(Qt::AlignCenter);
        DialogLayout->addWidget(Label);
        // TextView
        QPlainTextEdit *TextView=new QPlainTextEdit();
        TextView->setReadOnly(true);
        TextView->setWordWrapMode(QTextOption::NoWrap);
        DialogLayout->addWidget(std::move(TextView));
        TextView->appendPlainText(Content);
        dialog->setLayout(DialogLayout);
        // Show
        dialog->show();
    }
}
}

#endif
