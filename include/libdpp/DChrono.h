#ifndef DChronoH
#define DChronoH

#include <chrono>
#include <sstream>

namespace DTools
{
namespace DChrono
{
    unsigned long NowMillis(void);
    std::stringstream GetChronoInfo(void);
}
}

#endif
