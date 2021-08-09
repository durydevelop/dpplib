#include "libdpp/DChrono.h"

namespace DTools
{
namespace DChrono
{
    unsigned long NowMillis(void) {
        return(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        /*
        auto tpoint=std::chrono::system_clock::now();
        auto ms=std::chrono::time_point_cast<std::chrono::milliseconds>(tpoint);
        return(ms.time_since_epoch().count());
        */
    }
}
}
