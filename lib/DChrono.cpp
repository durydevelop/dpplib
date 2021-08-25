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

    std::stringstream GetChronoInfo(void) {
        std::stringstream ss;
        ss << "system_clock" << std::endl;
        ss << "period = " << std::chrono::system_clock::period::num << "/" << std::chrono::system_clock::period::den << std::endl;
        ss << "steady = " << std::chrono::system_clock::is_steady << std::endl;
        ss << "high_resolution_clock" << std::endl;
        ss << "period = " << std::chrono::high_resolution_clock::period::num << "/" << std::chrono::high_resolution_clock::period::den << std::endl;
        ss << "steady = " << std::chrono::high_resolution_clock::is_steady << std::endl;
        ss << "steady_clock" << std::endl;
        ss << "period = " << std::chrono::steady_clock::period::num << "/" << std::chrono::steady_clock::period::den << std::endl;
        ss << "steady = " << std::chrono::steady_clock::is_steady << std::endl;
        return ss;
    }
}
}
