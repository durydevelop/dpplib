//---------------------------------------------------------------------------
#ifndef DPathH
#define DPathH
//---------------------------------------------------------------------------
//#include "DCallback.hpp"
#include <chrono>
#include <functional>

// Macro ridefined from basestd.h
#define DIntToPtr(i) (void *)(uintptr_t)(i)

// Filesystem library include
#if __cplusplus > 201402L // C++17
	#if __GNUC__ >= 8
		#include <filesystem>
	#else
		#include <experimental/filesystem>
	#endif
#else
	#include <boost/filesystem.hpp>
#endif

namespace DTools
{
	namespace DPath {
		// Filesystem library namespace
		#if __cplusplus > 201402L // C++17
			#if __GNUC__ >= 8
				namespace fs = std::filesystem;
			#else
				namespace fs = std::experimental::filesystem;
			#endif
			namespace err = std;
		#else
			namespace fs=boost::filesystem;
			namespace err = boost::system;
		#endif

		#if defined _WIN32 || defined _WIN64
            // Only for Windows
			std::string GetFileVersion(std::string Filename=std::string(), bool TrimDots = true);
		#endif

		//typedef std::function<void(uint8_t,void*)> DCallback;
		typedef void (*DGlobalCallback)(uint8_t,void*);
		typedef void (*DMemberCallback)(void*,uint8_t,void*);

        fs::path GetExePath();
		fs::path ChangeExt(fs::path Path, std::string NewExt, bool Execute, err::error_code *ec = NULL);
		bool IsOlderThanHrs(const fs::path& Path, const int Hrs);
		std::chrono::system_clock::time_point LastWriteTime(const fs::path Path);
		std::vector<std::string> ReadAllExts(fs::path Path);
		std::uintmax_t space_to_be_freed(const fs::path& dir, unsigned int percent_free_required);
		err::error_code Copy_File(const std::string &From, const std::string &To, bool OverwriteExisting, bool SafeMode = false);
		err::error_code Copy_File(const fs::path &From, const fs::path &To, bool OverwriteExisting, bool SafeMode = false);
		err::error_code CopyDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DGlobalCallback Callback);
		err::error_code MoveDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DGlobalCallback Callback);
		err::error_code DeleteDir(fs::path Dir);
		int ListDirs(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll);
		int ListDirs(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive);
		int CountDirs(fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll);
		int CountDirs(fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive);
        int CountDirs(fs::path PathToScan, bool Recoursive);
        int ListFiles(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll);
        int ListFiles(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive);
        int CountFiles(fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll);
        int CountFiles(fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive);
		int CountFiles(fs::path PathToScan, bool Recoursive);
	}
}
#endif
