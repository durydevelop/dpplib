#ifndef DStringH
#define DStringH

#include <sstream>
#include <memory>
#include <libdpp/DVector.h>

namespace DTools
{
namespace DString
{
	class DStringList {
		/* TODO:
		 * _[]
		 * Insert
		 * Size
		 * Delete
		 */
		public:
			DStringList(){};
			void Add(std::string Str) {
				StringList.emplace_back(Str);
			}
			void AddVect(std::vector<std::string>&& List) {
				StringList.insert(StringList.end(),List.begin(),List.end());
			}
			std::string Join(std::string Pattern) {
				return(DVector::JoinToStr(StringList,Pattern));
			}
			void Clear(void) {
				//StringList.clear();
				StringList.resize(0);
				StringList.shrink_to_fit();
			}
			bool IsEmpty(void) {
				return(StringList.empty());
			}
		private:
			std::vector<std::string> StringList;
	};

	std::string& LTrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
	std::string& RTrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
	std::string& Trim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
	std::string& RemoveChars(std::string& str, const std::string& chars);
	std::string RemoveCharsCopy(std::string str, const std::string& chars);
	std::string& RemoveNotDigit(std::string& str);
	std::string RemoveNotDigitCopy(std::string str);
	std::string& RemoveCharsIfNot(std::string& str, const std::string& allowed);
    std::string RemoveCharsIfNotCopy(std::string str, const std::string& allowed);
    std::string& RemoveLStr(std::string& SourceStr, const std::string& StrToRemove);
    std::string& RemoveRStr(std::string& SourceStr, const std::string& StrToRemove);
    std::string& RemoveAllStr(std::string& SourceStr, const std::string& StrToRemove);
	std::string& CutR(std::string& str, size_t count);
	std::string CutRCopy(std::string str, size_t count);
	std::string& CutL(std::string& str, size_t count);
	std::string CutLCopy(std::string str, size_t count);
	bool IsNumber(const std::string& str);
	bool IsInteger(const std::string& str);
	bool IsUnsignedInteger(const std::string& str);
	bool IsDouble(const std::string& str);
	bool IsUnsignedDouble(const std::string& str);
    int ToInt(const std::string& str);

	template<class T>
	T ToNumber(const std::string& str) {
		T Num;
		//std::string sDigit=RemoveNotDigit_Copy(TypeFromArticle);
		std::istringstream(str) >> Num;
		return(Num);
	}

    // Strings Formatting
	template<typename ... Args>
	std::string FormatString(const std::string& format,Args ... args) {
		size_t len = snprintf(nullptr,0,format.c_str(),args ...)+1; // Extra space for '\0'
		std::unique_ptr<char[]> buf(new char[len]);
		snprintf(buf.get(),len, format.c_str(),args ...);
		return std::string(buf.get(),buf.get()+len-1); // We don't want the '\0' inside
	};

	// Convert
	std::string& ToUpper(std::string& str);
	std::string ToUpperCopy(std::string str);
	std::string& ToLower(std::string& str);
	std::string ToLowerCopy(std::string str);
	std::wstring ToWideStr(const std::string& str);
	std::string ToStdStr(const std::wstring& wstr);

	// Compare
	bool CmpNoCase(std::string str1, std::string str2);
	bool Contains(std::string str, std::string pattern, bool CaseSensitive = false);
    bool StartsWith(std::string str, std::string pattern, bool CaseSensitive = false);
    bool EndsWith(std::string str, std::string pattern, bool CaseSensitive = false);

	//std::string& RemoveAll(std::string& str, std::string& Search);
} // DString
} // DTools
#endif
