#include "libdpp/DString.h"
#include <regex>
#include <iomanip>
#include <codecvt>

namespace DTools
{
namespace DString
{
	std::string& LTrim(std::string& str, const std::string& chars) {
		str.erase(0, str.find_first_not_of(chars));
		return str;
	}

	std::string& RTrim(std::string& str, const std::string& chars) {
		str.erase(str.find_last_not_of(chars) + 1);
		return str;
	}

	std::string& Trim(std::string& str, const std::string& chars) {
		return LTrim(RTrim(str, chars), chars);
	}

	std::string& RemoveChars(std::string& str, const std::string& chars) {
		str.erase(remove_if(str.begin(), str.end(), [&chars](const char& c) {
			return chars.find(c) != std::string::npos;
		}), str.end());
		return str;
	}

	std::string RemoveCharsCopy(std::string str, const std::string& chars) {
		return RemoveChars(str, chars);
	}

	std::string& RemoveNotDigit(std::string& str) {
		str.erase(remove_if(str.begin(), str.end(), [](const char& c) {
			return !isdigit(c);
		}), str.end());
		return str;
	}

	std::string RemoveNotDigitCopy(std::string str) {
		return RemoveNotDigit(str);
	}

	std::string& RemoveCharsIfNot(std::string& str, const std::string& allowed) {
		str.erase(remove_if(str.begin(), str.end(), [&allowed](const char& c) {
			return allowed.find(c) == std::string::npos;
		}), str.end());
		return str;
	}

	std::string RemoveCharsIfNotCopy(std::string str, const std::string& allowed) {
		return RemoveCharsIfNot(str, allowed);
	}

	std::string& CutR(std::string& str, size_t count) {
		str=str.substr(0,str.size()-count);
		return(str);
	}

	std::string CutRCopy(std::string str, size_t count) {
		return(str=str.substr(0,str.size()-count));
	}

	std::string& CutL(std::string& str, size_t count) {
		str=str.substr(count,str.size()-count);
		return(str);
	}

	std::string CutLCopy(std::string str, size_t count) {
		return(str=str.substr(count,str.size()-count));
	}

	//! return true if str is any type on number
	bool IsNumber(const std::string& str) {
	  return std::regex_match(str,std::regex("-?[0-9]+([.][0-9]+)?"));
	}

	//! return true if str is an integer number
	bool IsInteger(const std::string& str) {
	  return std::regex_match(str,std::regex("[+-]?[0-9]+"));
	}

	//! return true if str is an unsigned integer number
	bool IsUnsignedInteger(const std::string& str) {
	  return std::regex_match(str,std::regex("[+]?[0-9]+"));
	}

	//! return true if str is a double number
	bool IsDouble(const std::string& str) {
	  return std::regex_match(str,std::regex("[+-]?[0-9]+[.]?[0-9]+"));
	}

	//! return true if str is an unsigned double number
	bool IsUnsignedDouble(const std::string& str) {
	  return std::regex_match(str,std::regex("[+]?[0-9]+[.]?[0-9]+"));
	}

	//! Create now date/time string according to format specify by https://en.cppreference.com/w/cpp/io/manip/put_time
	/**
	* @param format	->	Format string (default "%Y%m%d %H%M%S")
	**/
	std::string FormatNow(std::string format) {
		auto now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::stringstream ss;
		ss << std::put_time(localtime(&now_time_t),format.c_str());
		return(ss.str());
	}

	//! Create a date/time string from a time_t type according to format specify by https://en.cppreference.com/w/cpp/io/manip/put_time
	/**
	* @param in_time_t	->	time_t time to format
	* @param format		->	Format string (default "%Y%m%d %H%M%S")
	**/
	std::string FormatTimeT(time_t in_time_t, std::string format) {
		std::stringstream ss;
		ss << std::put_time(localtime(&in_time_t),format.c_str());
		return(ss.str());
	}

	//! Create a date/time string from a time_pont type according to format specify by https://en.cppreference.com/w/cpp/io/manip/put_time
	/**
	* @param in_time_t	->	time_point time to format
	* @param format		->	Format string (default "%Y%m%d %H%M%S")
	**/
	std::string FormatTimeP(std::chrono::system_clock::time_point in_time_point, std::string format) {
		auto in_time_t=std::chrono::system_clock::to_time_t(in_time_point);
		std::stringstream ss;
		ss << std::put_time(localtime(&in_time_t),format.c_str());
		return(ss.str());
	}

	//! Convert string to uppercase
	/**
	* @param str    ->  string to convert
	**/
	std::string& ToUpper(std::string& str) {
		std::transform(str.begin(), str.end(),str.begin(), ::toupper);
		return(str);
	}

	//! Convert string to uppercase
	/**
	* @param str    ->  string to convert
	* @return a converded copy of input string
	**/
	std::string ToUpperCopy(std::string str) {
		std::transform(str.begin(), str.end(),str.begin(), ::toupper);
		return(str);
	}

	//! Convert string to lowercase
	/**
	* @param str    ->  string to convert
	**/
	std::string& ToLower(std::string& str) {
		std::transform(str.begin(), str.end(),str.begin(), ::tolower);
		return(str);
	}

	//! Convert string to lowercase
	/**
	* @param str    ->  string to convert
	* @return a converded copy of input string
	**/
	std::string ToLowerCopy(std::string str) {
		std::transform(str.begin(), str.end(),str.begin(), ::tolower);
		return(str);
	}

    //! Compare strings case-unsensitive
    /**
     * @param str1  ->  first string to compare
     * @param str2  ->  first string to compare
     * @return true if string are equals, otherwise false
     */
    bool CmpNoCase(std::string str1, std::string str2) {
        std::transform(str1.begin(),str1.end(),str1.begin(),::tolower);
        std::transform(str2.begin(),str2.end(),str2.begin(),::tolower);
        if (str1.compare(str2) == 0) {
            return true;
        }
        return false;
    }

    /**
     * @brief Check str contains pattern.
     * @param str       ->  Source string.
     * @param pattern   ->  Pattern string to search for.
     * @param CaseSensitive ->  Set to false to check case-unsensitive.
     * @return true if pattern is contained in str (also if theay are equals).
     */
    bool Contains(std::string str, std::string pattern, bool CaseSensitive) {
        if (CaseSensitive) {
            std::transform(str.begin(),str.end(),str.begin(),::tolower);
            std::transform(pattern.begin(),pattern.end(),pattern.begin(),::tolower);
        }

        if (str.find(pattern) != std::string::npos) {
            return true;
        }
        return false;
    }

    std::wstring ToWideStr(const std::string& str) {
        using convert_typeX = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> converterX;
        return converterX.from_bytes(str);
    }

    std::string ToStdStr(const std::wstring& wstr) {
        using convert_typeX = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> converterX;
        return converterX.to_bytes(wstr);
    }

	/*
	//! Erase all occurrences of characters in a string
	std::string& RemoveAll(std::string& str, std::string& Search) {
		str.erase(std::remove(str.begin(), str.end(), 'a'), str.end());
		return(str);
	}
	*/
} // DString
} // DTools
