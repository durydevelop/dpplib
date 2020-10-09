#include "DWindow.h"
#include "DFilesystem.h"
#include "DPath.h"

#ifdef QT_GUI_LIB
    #include <QWidget>
#endif

namespace DTools
{
    namespace DWindow {
        /**
         * @brief Save window position data
         * @details Save position and size of a window using DTools::DPreferences::DPreferences class
         * @param Name      ->  Window name
         * @param X         ->  X position
         * @param Y         ->  Y position
         * @param Width     ->  Window width
         * @param Height    ->  Windows height
         * @param DestPrefs ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created
         * @return true on success, false if cannot store data into prefs
         */
        bool SaveWindowPositionData(std::string Name, size_t X, size_t Y, size_t Width, size_t Height, DTools::DPreferences::DPreferences *DestPrefs) {
            DTools::DPreferences::DPreferences *Prefs=nullptr;
            if (DestPrefs == nullptr) {
                std::string PrefFilename=DTools::DPath::GetExePath().string();
                Prefs=new DTools::DPreferences::DPreferences(PrefFilename+".conf");
                if (!Prefs->IsReady()) {
                    return false;
                }
            }
            else {
                Prefs=DestPrefs;
            }

            Prefs->WriteInteger("Windows."+Name,"X",X);
            Prefs->WriteInteger("Windows."+Name,"Y",Y);
            Prefs->WriteInteger("Windows."+Name,"Width",Width);
            Prefs->WriteInteger("Windows."+Name,"Height",Height);
            Prefs->Save();

            if (DestPrefs == nullptr && Prefs != nullptr) {
                delete Prefs;
            }

            return true;
        }

        /**
         * @brief Restore window position data
         * @details Extract position and size of a window using DTools::DPreferences::DPreferences class
         * @param Name      ->  Window name
         * @param X         ->  Reference to X variable to fill with extraced X position
         * @param Y         ->  Reference to y variable to fill with extraced Y position
         * @param Width     ->  Reference to Width variable to fill with extraced width
         * @param Height    ->  Reference to Height variable to fill with extraced height
         * @param DestPrefs ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created
         * @return true on success, false if cannot store data into prefs
         */
        bool RestoreWindowPositionData(std::string Name, int& X, int& Y, int& Width, int& Height, DTools::DPreferences::DPreferences *DestPrefs) {
            DTools::DPreferences::DPreferences *Prefs=nullptr;
            if (DestPrefs == nullptr) {
                std::string PrefFilename=DTools::DPath::GetExePath().string();
                Prefs=new DTools::DPreferences::DPreferences(PrefFilename+".conf");
                if (!Prefs->IsReady()) {
                    return false;
                }
            }
            else {
                Prefs=DestPrefs;
            }
            X=Prefs->ReadInteger("Windows."+Name,"X",0);
            Y=Prefs->ReadInteger("Windows."+Name,"Y",0);
            Width=Prefs->ReadInteger("Windows."+Name,"Width",0);
            Height=Prefs->ReadInteger("Windows."+Name,"Height",0);

            if (DestPrefs == nullptr && Prefs != nullptr) {
                delete Prefs;
            }
            return  true;
        }

        #ifdef QT_GUI_LIB
        //************************ Qt versions ************************

        /**
         * @brief Save QWindow position data
         * @details Save position and size of a QWindow using DTools::DPreferences::DPreferences class
         * @param qWindow   ->  Reference to a QWindow object
         * @param DestPrefs ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created
         * @return true on success, false if cannot store data into prefs
         */
        bool SaveQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences::DPreferences *DestPrefs) {
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
        bool SaveQWindowPosition(QDialog& qDialog, DTools::DPreferences::DPreferences *DestPrefs) {
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
        bool RestoreQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences::DPreferences *DestPrefs) {
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
        bool RestoreQWindowPosition(QDialog& qDialog, DTools::DPreferences::DPreferences *DestPrefs) {
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
    }
}
