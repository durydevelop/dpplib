//---------------------------------------------------------------------------
#ifndef DStringH
#define DStringH
//---------------------------------------------------------------------------
#include <chrono>
#include <sstream>
#include <memory>

namespace DTools
{
	enum DProgressCode {INC_FILE=1,     //Trigger when a file is copied or moved
						INC_DIR=2,      //Trigger when a file is copied or moved
						SET_FILES=3,    //Trigger to send nr of files to copy or move
						SET_DIRS=4,     //Trigger to send nr of dirs to copy or move
						SET_OBJ=5,      //Trigger to send nr of dirs+file to copy or move
						};

	namespace DString
	{
		std::string& LTrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
		std::string& RTrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
		std::string& Trim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
		std::string& RemoveChars(std::string& str, const std::string& chars);
		std::string RemoveCharsCopy(std::string str, const std::string& chars);
		std::string& RemoveNotDigit(std::string& str);
		std::string RemoveNotDigitCopy(std::string str);
		std::string& RemoveCharsIfNot(std::string& str, const std::string& allowed);
		std::string RemoveCharsIfNot_Copy(std::string str, const std::string& allowed);
		bool IsNumber(const std::string& str);
		bool IsInteger(const std::string& str);
		bool IsUnsignedInteger(const std::string& str);
		bool IsDouble(const std::string& str);
		bool IsUnsignedDouble(const std::string& str);

		template<class T>
		T ToNumber(const std::string& str) {
			T Num;
			//std::string sDigit=RemoveNotDigit_Copy(TypeFromArticle);
			std::istringstream(str) >> Num;
            return(Num);
        }

		//! Strings Format
		template<typename ... Args>
		std::string FormatString(const std::string& format,Args ... args) {
			size_t len = snprintf(nullptr,0,format.c_str(),args ...)+1; // Extra space for '\0'
			std::unique_ptr<char[]> buf(new char[len]);
			snprintf(buf.get(),len, format.c_str(),args ...);
			return std::string(buf.get(),buf.get()+len-1); // We don't want the '\0' inside
		};
		std::string FormatNow(std::string format);
		std::string FormatTimeT(time_t in_time_t, std::string format);
        std::string FormatTimeP(std::chrono::system_clock::time_point in_time_point, std::string format);
        std::string& ToUpper(std::string& str);
        std::string ToUpperCopy(std::string str);
        std::string& ToLower(std::string& str);
        std::string ToLowerCopy(std::string str);
        bool CmpNoCase(std::string str1, std::string str2);
        //std::string& RemoveAll(std::string& str, std::string& Search);
	}
}
#endif
