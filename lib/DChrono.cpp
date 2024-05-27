#include "dpplib/DChrono.h"
#include <boost/date_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using boost::posix_time::time_input_facet;

// TODO: chrono::file_time

namespace DTools
{
namespace DChrono
{
    //! List of Known input time formats used in AntToTimeT().
    const std::vector<std::locale> KnownInputFormats = {
        std::locale(std::locale::classic(), new time_input_facet("%Y-%m-%d %H:%M:%S")),
        std::locale(std::locale::classic(), new time_input_facet("%d/%m/%Y %H:%M:%S")),
        std::locale(std::locale::classic(), new time_input_facet("%Y%m%d%H%M%S")),
        std::locale(std::locale::classic(), new time_input_facet("%Y%m%d%H%M")),
        std::locale(std::locale::classic(), new time_input_facet("%Y-%m-%d-%H-%M"))
    };

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
     * @param DateTimeString    ->	Date time string to decode.
     * @param Fmt       		->	Format of DateTimeString.
	 * @return a time_t containing decoded timeString.
	 * * example:
	 * @code
	 * std::time_t tt=to_time_t("01/12/2021 10:20:05","%d/%m/%Y %H:%M:%S");
	 * @code
	 * format rappresent the way that timeString is witten.
	 * TODO: test "%T"
     */
/*
    std::time_t AnyToTimeT(const std::string& DateTimeString) {
        for (const std::string& InputFmt : KnownDateTimeFormats) {
            std::time_t t=ToTimeT(DateTimeString,InputFmt);
            if (t != -1) {
                std::string s="Converted with "+InputFmt;
                return t;
            }
        }
        return (-1);
    }
*/
    std::time_t ToTimeT(const std::string& DateTimeString, const std::string& Fmt) {
        /*
        std::istringstream ss(DateTimeString);
        ss.imbue(std::locale());
		struct std::tm tm;
        ss >> std::get_time(&tm,Fmt.c_str());
        tm.tm_isdst=1;
        return(std::mktime(&tm));
        */

        std::istringstream ss(DateTimeString);

        ss.imbue(std::locale(std::locale::classic(), new time_input_facet(Fmt)));
        boost::posix_time::ptime this_time;
        ss >> this_time;

        if(this_time != boost::posix_time::not_a_date_time) {
            return PtimeToTimeT(this_time);
        }

        return (-1);
	}

// TODO
    /*
     * "EEE, dd MMM yyyy HH:mm:ss '('ZZZ')'",
    "EEE, dd MMM yyyy HH:mm:ss '\"'ZZZ'\"'",
    "EEE, dd MMM yyyy hh:mm:ss z",
    "EEE, dd MMM yyyy HH:mm Z",
    "EEE, dd MMM yyyy HH:mm:ss",
    "EEE, MMM dd, yyyy 'at' HH:mm:ss z",
    "EEE M/dd/yyyy hh:mm:ss a",
    "EEE MMM dd HH:mm:ss z yyyy",
    "EEE MMM dd hh:mm:ss yyyy",
    "EEEE, MMMM dd, yyyy hh:mm:ss a",
    "EEEE, MMMM dd, yyyy HH:mm a",
    "EEEE, MMMM dd, yyyy HH:mm",
    "MMM dd, yyyy hh:mm:ss a",
    "dd MMM yyyy hh:mm:ss z",
    "dd-MMM-yyyy HH:mm:ss z",
    "dd MMM yy HH:mm:ss",
    "MM/dd/yyyy  hh:mm a  (EEEE)",
    "MM/dd/yyyy hh:mm a (EEEE)",
    "MM/dd/yyyy hh:mm:ss",
    "MM/dd/yyyy hh:mm a Z",
    "MM/dd/yyyy hh:mma Z",
    "MM/dd/yyyy hh:mma",
    "MM/dd/yyyy hh:mm a",
    "MM/dd/yyyy hh:mm Z",
    "MM/dd/yy hh:mm a Z",
    "MM/dd/yy hh:mma Z",
    "MM/dd/yy HH:mm a",
    "MM/dd/yy HH:mm Z",
    "MM/dd/yyyy",
    "yyyy-MM-dd HH:mm:ss",
    "yyyyMMddhhmmss",
    "yyyyMMddhhmm",
    "yyyyMMdd"
*/
/*
 * // TZ Lib
 * #include "date/tz.h"
#include <iostream>

int
main()
{
    using namespace date;
    using namespace std::chrono_literals;
    auto zt = make_zoned(current_zone(), local_seconds{1470003841s});
    std::cout << zt.get_sys_time().time_since_epoch() << '\n'; // 1470018241s
    std::cout << zt << '\n';
    std::cout << zt.get_sys_time() << " UTC\n";
}

// C++20
#include <chrono>
#include <iostream>

int
main()
{
    using namespace std::chrono;
    zoned_time zt{current_zone(), local_seconds{1470003841s}};
    std::cout << zt.get_sys_time().time_since_epoch() << '\n'; // 1470018241s
    std::cout << zt << '\n';
*/
    /**
     * @brief Convert any of KnownInputFormats in a local time_t type.
     * @param DateTimeString    ->  Date time string to convert.
     * @return a time_t according to local time zone.
     */
    std::time_t AnyToTimeT(const std::string& DateTimeString) {
        //const size_t FormatsCount = sizeof(KnownInputFormats)/sizeof(KnownInputFormats[0]);
        //for(size_t ixF=0; ixF<FormatsCount; ++ixF)
        for (auto InputFmt : KnownInputFormats) {
            std::istringstream ss(DateTimeString);
            ss.imbue(InputFmt);
            boost::posix_time::ptime this_time;
            ss >> this_time;

            if(this_time != boost::posix_time::not_a_date_time) {
                return PtimeToTimeT(this_time);
            }
        }

        return(-1);
    }

    /**
     * @brief Convert a boost::posix_time::ptime to a loca time_t.
     * @param pt    ->  boost::posix_time::ptime ptime to convert.
     * @return a time_t according to local time zone.
     */
    time_t PtimeToTimeT (const boost::posix_time::ptime& pt) {
        using namespace boost::posix_time;
        using namespace boost::local_time;
        // This is needed if the time zone changes after the program starts.
        // _tzset();
        tzset ();

        // Grab copies of the current time in local and UTC form.
        auto p_time = microsec_clock::universal_time (); // UTC.
        auto lp_time = boost::date_time::c_local_adjustor<ptime>::utc_to_local (p_time);

        // Calculate the difference between the UTC and local time and put it in a string.
        // This will yield "-07:00:00" for PST and "-08:00:00" for PDT.
        auto time_diff = to_simple_string (lp_time - p_time);

        // Convert the ptime to a local_date_time object using the local time zone.
        // We will create the time zone with the time difference string generated above.
        local_date_time local_time (p_time, time_zone_ptr (new posix_time_zone (time_diff)));

        // Calculate the difference in milliseconds between the UTC and local time.
        auto time_t_diff = to_time_t (p_time) - to_time_t (local_time.local_time ());

        // Return the given time in ms plus the time_t difference (to get the UTC ms).
        return to_time_t (pt) + time_t_diff;
    }

    /**
     * @brief Create now date/time string according to format specify by https://en.cppreference.com/w/cpp/io/manip/put_time
     * @param Fmt	->	Format string (default "%Y%m%d %H%M%S")
     * @return * @return a string as Fmt specs or an emty spring is TimeT is invalid (-1).
     */
    std::string FormatNow(std::string Fmt) {
        auto now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(localtime(&now_time_t),Fmt.c_str());
        return(ss.str());
    }

    /**
     * @brief Create a date/time string from a time_t type according to format specify by https://en.cppreference.com/w/cpp/io/manip/put_time
     * @param TimeT	->	time_t time to format
     * @param Fmt		->	Format string (default "%Y%m%d %H%M%S")
     * @return a string as Fmt specs or an emty spring is TimeT is invalid (-1).
     */
    std::string FormatTimeT(time_t TimeT, std::string Fmt) {
        if (TimeT == -1) {
            return (std::string());
        }
        std::stringstream ss;
        ss << std::put_time(localtime(&TimeT),Fmt.c_str());
        return(ss.str());
    }

    /**
     * @brief Create a date/time string from a time_pont type according to format specify by https://en.cppreference.com/w/cpp/io/manip/put_time
     * @param TimeT	->	time_point time to format
     * @param Fmt	->	Format string (default "%Y%m%d %H%M%S")
     * @return
     */
    std::string FormatTimeP(std::chrono::system_clock::time_point TimePoint, std::string Fmt) {
        auto TimeT=std::chrono::system_clock::to_time_t(TimePoint);
        std::stringstream ss;
        ss << std::put_time(localtime(&TimeT),Fmt.c_str());
        return(ss.str());
    }
}
}
