#include "dpplib/DWindow.h"
#include "dpplib/DPath.h"

namespace DTools
{
namespace DWindow
{
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
    bool SaveWindowPositionData(std::string Name, size_t X, size_t Y, size_t Width, size_t Height, DTools::DPreferences *DestPrefs) {
        DTools::DPreferences *Prefs=nullptr;
        if (DestPrefs == nullptr) {
            std::string PrefFilename=DTools::DPath::GetExeFilename().string();
            Prefs=new DTools::DPreferences(PrefFilename+".conf");
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
        bool ret=Prefs->Save();

        if (DestPrefs == nullptr && Prefs != nullptr) {
            delete Prefs;
        }

        return ret;
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
    bool RestoreWindowPositionData(std::string Name, int& X, int& Y, int& Width, int& Height, DTools::DPreferences *DestPrefs) {
        DTools::DPreferences *Prefs=nullptr;
        if (DestPrefs == nullptr) {
            std::string PrefFilename=DTools::DPath::GetExeFilename().string();
            Prefs=new DTools::DPreferences(PrefFilename+".conf");
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
} // DWindow
} // DTools
