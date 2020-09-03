#include "DWindow.h"
#include "DFilesystem.h"
#include "DPath.h"

#ifdef QT_GUI_LIB
    #include <QSettings>
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
        bool SaveWindowPosition(std::string Name, size_t X, size_t Y, size_t Width, size_t Height, DTools::DPreferences::DPreferences *DestPrefs) {
            DTools::DPreferences::DPreferences *Prefs=nullptr;
            if (DestPrefs == nullptr) {
                std::string PrefFilename=DTools::DPath::GetExePath();
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
        bool RestoreWindowPosition(std::string Name, int& X, int& Y, int& Width, int& Height, DTools::DPreferences::DPreferences *DestPrefs) {
            DTools::DPreferences::DPreferences *Prefs=nullptr;
            if (DestPrefs == nullptr) {
                std::string PrefFilename=DTools::DPath::GetExePath();
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
        bool SaveQWindowPosition(QWindow& qWindow, DTools::DPreferences::DPreferences *DestPrefs) {
            return(SaveWindowPosition(qWindow.objectName().toStdString(),qWindow.x(),qWindow.y(),qWindow.width(),qWindow.height(),DestPrefs));
        }

        /**
         * @brief Restore QWindow position data
         * @details Extract position and size of a QWindow using DTools::DPreferences::DPreferences class
         * @param qWindow   ->  Reference to a QWindow object
         * @param DestPrefs ->  Pointer to DTools::DPreferences::DPreferences instance used to save data. If no DPreferences is provided (DestPrefs is nullptr) a new file named "executable-name.conf" is created
         * @return true on success, false if cannot store data into prefs
         */
        bool RestoreQWindowPosition(QWindow& qWindow, DTools::DPreferences::DPreferences *DestPrefs) {
            int X,Y,Width,Height;
            if (!DTools::DWindow::RestoreWindowPosition(qWindow.objectName().toStdString(),X,Y,Width,Height,DestPrefs)) {
                    return false;
            }
            qWindow.setX(X);
            qWindow.setY(Y);
            if (Width > 0) {
                qWindow.setWidth(Width);
            }
            if (Height > 0) {
                qWindow.setHeight(Height);
            }
            return true;
        }

        /**
         * @brief Save QMainWindow geometry (pure Qt api version)
         * @details Save QMainWindow geometry and state (menus, info bars, ...) information using QSettings api
         * @param qWindow       ->  Reference to a QMainWindow object
         * @param CompanyName   ->  Used to store informations in correct way. If empty QSettings default rules are used
         * @param AppName       ->  Used to store informations in correct way. If empty QSettings default rules are used
         */
        void SaveQWindowGeometry(QMainWindow& qWindow, QString CompanyName, QString AppName) {
            QSettings qSettings(CompanyName,AppName);
            qSettings.setValue(qWindow.objectName()+"/Geometry",qWindow.saveGeometry());
            qSettings.setValue(qWindow.objectName()+"/State",qWindow.saveState());
        }

        /**
         * @brief Restore QMainWindow geometry (pure Qt api version)
         * @details Restore QMainWindow geometry and state (menus, info bars, ...)
         * @param qWindow       ->  Reference to a QMainWindow object
         * @param CompanyName   ->  Used to store informations in correct way. If empty QSettings default rules are used
         * @param AppName       ->  Used to store informations in correct way. If empty QSettings default rules are used
         */
        void RestoreQWindowGeometry(QMainWindow& qWindow, QString CompanyName, QString AppName) {
            QSettings qSettings(CompanyName,AppName);
            qWindow.restoreGeometry(qSettings.value(qWindow.objectName()+"/Geometry").toByteArray());
            qWindow.restoreState(qSettings.value(qWindow.objectName()+"/State").toByteArray());
        }

        /**
         * @brief Save Qt Window geometry (pure Qt api version)
         * @details Save Qt Window geometry information using QSettings api
         * @param qWindow       ->  Reference to a QWidget object. QWidget it is used because QWindow descend from it
         * @param CompanyName   ->  Used to store informations in correct way. If empty QSettings default rules are used
         * @param AppName       ->  Used to store informations in correct way. If empty QSettings default rules are used
         */
        void SaveQWindowGeometry(QWidget& qWindow, QString CompanyName, QString AppName) {
            QSettings qSettings(CompanyName,AppName);
            qSettings.setValue(qWindow.objectName()+"/Geometry",qWindow.saveGeometry());
        }

        /**
         * @brief Restore Qt Window geometry (pure Qt api version)
         * @details Save Qt Window geometry information using QSettings api
         * @param qWindow       ->  Reference to a QWidget object. QWidget it is used because QWindow descend from it
         * @param CompanyName   ->  Used to store informations in correct way. If empty QSettings default rules are used
         * @param AppName       ->  Used to store informations in correct way. If empty QSettings default rules are used
         */
        void RestoreQWindowGeometry(QWidget& qWindow, QString CompanyName, QString AppName) {
            QSettings qSettings(CompanyName,AppName);
            qWindow.restoreGeometry(qSettings.value(qWindow.objectName()+"/Geometry").toByteArray());
        }
        #endif
    }
}
