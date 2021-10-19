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
