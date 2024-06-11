#ifndef DWindowH
#define DWindowH

#include <dpplib/DPreferences.h>
#include <string>

#ifdef QT_GUI_LIB
    #include <dpplib/qt/DQWindow.h>
#endif

namespace DTools
{
namespace DWindow
{
    bool SaveWindowPositionData(std::string Name, size_t X, size_t Y, size_t Width, size_t Height, DTools::DPreferences *DestPrefs = nullptr);
    bool RestoreWindowPositionData(std::string Name, int& X, int& Y, int& Width, int& Height, DTools::DPreferences *DestPrefs = nullptr);
} // DWindow
} // DTools

#endif
