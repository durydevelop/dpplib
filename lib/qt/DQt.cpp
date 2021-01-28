#include "libdpp/qt/DQt.h"
#ifdef QT_GUI_LIB
        //************************ Qt versions ************************

        /**
         * @brief Save QWindow position data
         * @details Save position and size of a QWindow using DTools::DPreferences::DPreferences class
         * @param qWindow   ->  Reference to a QWindow object
         * @param DestPrefs ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created
         * @return true on success, false if cannot store data into prefs
         */
        bool SaveQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences *DestPrefs) {
            QWidget& qWidget=(QWidget&) qMainWindow;
            return(SaveWindowPositionData(qWidget.objectName().toStdString(),qWidget.x(),qWidget.y(),qWidget.width(),qWidget.height(),DestPrefs));
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
            return(SaveWindowPositionData(qWidget.objectName().toStdString(),qWidget.x(),qWidget.y(),qWidget.width(),qWidget.height(),DestPrefs));
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
            if (X != 0 && Y != 0) {
                qMainWindow.move(X,Y);
            }
            if (Width > 0 && Height > 0) {
                qMainWindow.resize(Width,Height);
            }
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
#endif
