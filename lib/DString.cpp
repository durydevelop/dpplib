#include "dpplib/DString.h"
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

	/**
	 * @brief Trim
	 * @param str
	 * @param chars
	 * @return
	 */
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

    /**
     * @brief Remove first occurrence of a string from an other.
     * @param SourceStr     ->  Given string.
     * @param StrToRemove   ->  STring to remove.
     * @return a reference to given (modificated) string.
     */
    std::string& RemoveLStr(std::string& SourceStr, const std::string& StrToRemove) {
        size_t pos=SourceStr.find(StrToRemove);
        if (pos != std::string::npos) {
            SourceStr.erase(pos,StrToRemove.size());
        }
        return SourceStr;
    }

    /**
     * @brief Remove last occurrence of a string from an other.
     * @param SourceStr     ->  Given string.
     * @param StrToRemove   ->  STring to remove.
     * @return a reference to given (modificated) string.
     */
    std::string& RemoveRStr(std::string& SourceStr, const std::string& StrToRemove) {
        size_t pos=SourceStr.rfind(StrToRemove);
        if (pos != std::string::npos) {
            SourceStr.erase(pos,StrToRemove.size());
        }
        return SourceStr;
    }

    /**
     * @brief Remove all occurrences of a string from an other.
     * @param SourceStr     ->  Given string.
     * @param StrToRemove   ->  STring to remove.
     * @return a reference to given (modificated) string.
     */
    std::string& RemoveAllStr(std::string& SourceStr, const std::string& StrToRemove) {
        size_t len=StrToRemove.size();
        for (auto i=SourceStr.find(StrToRemove); i!=std::string::npos; i=SourceStr.find(StrToRemove)) {
            SourceStr.erase(i,len);
        }
        return SourceStr;
    }

	std::string& CutR(std::string& str, size_t count) {
        if (str.empty()) {
			return(str);
        }
        str=str.substr(0,str.size()-count);
		return(str);
	}

	std::string CutRCopy(std::string str, size_t count) {
		if (str.empty()) {
			return(str);
		}
		return(str=str.substr(0,str.size()-count));
	}

	std::string& CutL(std::string& str, size_t count) {
		if (str.empty()) {
			return(str);
		}
		str=str.substr(count,str.size()-count);
		return(str);
	}

	std::string CutLCopy(std::string str, size_t count) {
		if (str.empty()) {
			return(str);
		}
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

    //! Fastest naive conversion to int
    int ToInt(const std::string& str) {
        const char *p=str.c_str();
        int x = 0;
        bool neg = false;
        if (*p == '-') {
            neg = true;
            ++p;
        }
        while (*p >= '0' && *p <= '9') {
            x = (x*10) + (*p - '0');
            ++p;
        }
        if (neg) {
            x = -x;
        }
        return x;
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
     * @brief Check if str contains pattern.
     * @param str       ->  Source string.
     * @param pattern   ->  Pattern string to search for.
     * @param CaseSensitive ->  Set to false to check case-unsensitive.
     * @return true if pattern is contained in str (also if theay are equals).
     */
    bool Contains(std::string str, std::string pattern, bool CaseSensitive) {
        if (!CaseSensitive) {
            std::transform(str.begin(),str.end(),str.begin(),::tolower);
            std::transform(pattern.begin(),pattern.end(),pattern.begin(),::tolower);
        }

        if (str.find(pattern) != std::string::npos) {
            return true;
        }
        return false;
    }

    /**
     * @brief Check if #str starts with #pattern.
     * @param str       ->  Source string.
     * @param pattern   ->  Pattern string to search for.
     * @param CaseSensitive ->  Set to false to check case-unsensitive.
     * @return true if #str starts with #pattern (also if theay are equals).
     */
    bool StartsWith(std::string str, std::string pattern, bool CaseSensitive) {
        if (!CaseSensitive) {
            std::transform(str.begin(),str.end(),str.begin(),::tolower);
            std::transform(pattern.begin(),pattern.end(),pattern.begin(),::tolower);
        }

        size_t pos=str.find(pattern);
        if (pos == 0) {
            return true;
        }
        return false;
    }

    /**
     * @brief Check if #str ends with #pattern.
     * @param str       ->  Source string.
     * @param pattern   ->  Pattern string to search for.
     * @param CaseSensitive ->  Set to false to check case-unsensitive.
     * @return true if #str ends with #pattern (also if theay are equals).
     */
    bool EndsWith(std::string str, std::string pattern, bool CaseSensitive) {
        if (!CaseSensitive) {
            std::transform(str.begin(),str.end(),str.begin(),::tolower);
            std::transform(pattern.begin(),pattern.end(),pattern.begin(),::tolower);
        }

        size_t pos=str.rfind(pattern);
        if (pos == str.size()-pattern.size()) {
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
