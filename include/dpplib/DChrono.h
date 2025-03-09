#ifndef DChronoH
#define DChronoH

#include <chrono>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace DTools
{
namespace DChrono
{
    std::time_t Now(void);
    unsigned long NowMillis(void);
    std::stringstream GetChronoInfo(void);
    std::time_t ToTimeT(const std::string& DateTimeString, const std::string& Fmt);
    std::time_t AnyToTimeT(const std::string& DateTimeString);
    time_t PtimeToTimeT (const boost::posix_time::ptime& pt);
    std::string FormatNow(std::string Fmt = "%Y%m%d%H%M%S");
    std::string FormatTimeT(time_t TimeT, std::string Fmt = "%Y%m%d%H%M%S");
    std::string FormatTimeP(std::chrono::system_clock::time_point TimePoint, std::string Fmt = "%Y%m%d%H%M%S");
    int ExtractHour(std::time_t TimeT);
    int ExtractMin(std::time_t TimeT);
    int ExtractSec(std::time_t TimeT);

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
