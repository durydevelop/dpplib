#ifndef DChronoH
#define DChronoH

#include <chrono>
#include <sstream>
#include <iomanip>

namespace DTools
{
namespace DChrono
{
    unsigned long NowMillis(void);
    std::stringstream GetChronoInfo(void);
    std::time_t to_time_t(const std::string& timeString, const std::string& format);

	/**
	 * @brief Convert a time type to a time_t type (eg. a file_time_type)
	 * @param tp	->	Any time type that can be casted with chrono::syetem_clock
	 * @return a converted time_t.
	 */
	template <typename TP>
	std::time_t to_time_t(TP tp) {
		using namespace std::chrono;
		auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
		return system_clock::to_time_t(sctp);
	}
}
}

#endif
