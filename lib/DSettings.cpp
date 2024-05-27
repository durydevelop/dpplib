#include "dpplib/DSettings.h"
#include "dpplib/DPath.h"

// Common settings items
#define ITEM_SETTINGS           "settings"
#define DIR_DATA                "data"
#define DIR_LOG                 "log"

namespace fs=DTools::fs;

namespace DTools
{
    DSettings::DSettings(bool CreateLogDir, bool CreateDataDir)
    {
        StationID=0;
        
        if (CreateDataDir) {
            DTools::err::error_code ec;
            DataDir=fs::current_path() / DIR_DATA;
            if (!DTools::DPath::Exists(DataDir)) {
                LastStatus.append("Create DataDir: ");
                fs::create_directory(DataDir,ec);
                if (ec.value() == 0) {
                    LastStatus.append("OK");
                }
                else {
                    LastStatus.append("ERROR "+ec.message());
                }
            }
        }

        if (CreateLogDir) {
            DTools::err::error_code ec;
            LogDir=fs::current_path() / DIR_LOG;
            if (!DTools::DPath::Exists(LogDir)) {
                LastStatus.append("Create LogDir: ");
                fs::create_directory(LogDir,ec);
                if (ec.value() == 0) {
                    LastStatus.append("OK");
                }
                else {
                    LastStatus.append("ERROR "+ec.message());
                }
            }
        }
    }

    DSettings::~DSettings()
    {

    }

}
