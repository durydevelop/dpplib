#ifndef DSETTINGS_H
#define DSETTINGS_H

#include <string>
#include "libdpp/DFilesystem.h"

namespace DTools
{
class DSettings
{
    public:
        DSettings(bool CreateLogDir = false, bool CreateDataDir = false);
        virtual ~DSettings();

        std::string StationName;
        int StationID;
        DTools::fs::path DataDir;
        DTools::fs::path LogDir;
        std::string LastStatus;
};
}

#endif
