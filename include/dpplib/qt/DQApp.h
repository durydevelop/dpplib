#ifndef DQAppH
#define DQAppH

#ifdef QT_GUI_LIB

#include <dpplib/DFilesystem.h>

namespace DTools
{
namespace DApp
{
    bool SetStyleSheetFromFile(fs::path QssFilename);
}
}
#endif
#endif
