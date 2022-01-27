#include "libdpp/DChrono.h"

// TODO: chrono::file_time

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

    //! @return string informations about all system clock
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

	/**
	 * @brief Conver a time string specified by a format into time_t.
	 * @param timeString	->	Date time string to decode.
	 * @param format		->	Format of timeString.
	 * @return a time_t containing decoded timeString.
	 * * example:
	 * @code
	 * std::time_t tt=to_time_t("01/12/2021 10:20:05","%d/%m/%Y %H:%M:%S");
	 * @code
	 * format rappresent the way that timeString is witten.
	 * TODO: test "%T"
	 */
	std::time_t to_time_t(const std::string& timeString, const std::string& format) {
		std::istringstream ss(timeString);
		struct std::tm tm;
		ss >> std::get_time(&tm,format.c_str());
		return(std::mktime(&tm));
	}
}
}

// TODO:
/*
template
<
  typename DstTimePointT,
  typename SrcTimePointT,
  typename DstDurationT = typename DstTimePointT::duration,
  typename SrcDurationT = typename SrcTimePointT::duration,
  typename DstClockT = typename DstTimePointT::clock,
  typename SrcClockT = typename SrcTimePointT::clock
>
DstTimePointT
clock_cast_2nd(const SrcTimePointT tp,
               const SrcDurationT tolerance = std::chrono::nanoseconds {100},
               const int limit = 10)
{
  assert(limit > 0);
  auto itercnt = 0;
  auto src_now = SrcTimePointT {};
  auto dst_now = DstTimePointT {};
  auto epsilon = detail::max_duration<SrcDurationT>();
  do
    {
      const auto src_before = SrcClockT::now();
      const auto dst_between = DstClockT::now();
      const auto src_after = SrcClockT::now();
      const auto src_diff = src_after - src_before;
      const auto delta = detail::abs_duration(src_diff);
      if (delta < epsilon)
        {
          src_now = src_before + src_diff / 2;
          dst_now = dst_between;
          epsilon = delta;
        }
      if (++itercnt >= limit)
        break;
    }
  while (epsilon > tolerance);
#ifdef GLOBAL_ITERATION_COUNTER
  GLOBAL_ITERATION_COUNTER = itercnt;
#endif
  return dst_now + (tp - src_now);
}
*/
