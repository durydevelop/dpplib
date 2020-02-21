#include "DCommon.hpp"
#include "DCallback.hpp"
#include <chrono>

namespace DTools
{
	namespace DPath {
		fs::path ChangeExt(fs::path Path, std::string NewExt, bool Execute, err::error_code *ec = NULL);
		bool IsOlderThanHrs(const fs::path& Path, const int Hrs);
		std::chrono::system_clock::time_point LastWriteTime(const fs::path Path);
		std::vector<std::string> ReadAllExts(fs::path Path);
		err::error_code Copy_File(const std::string &From, const std::string &To, bool OverwriteExisting);
		err::error_code CopyDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DGlobalCallback *Callback);
		err::error_code MoveDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DGlobalCallback *Callback);
		err::error_code DeleteDir(fs::path Dir);
		int ListDirs(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll = false);
		int ListDirs(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive);
		int CountDirs(fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll = false);
		int CountDirs(fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive);
        int CountDirs(fs::path PathToScan, bool Recoursive);
		int ListFiles(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll = false);
		int ListFiles(std::vector<fs::path> *Result, fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive);
		int CountFiles(fs::path PathToScan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll = false);
		int CountFiles(fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive);
		int CountFiles(fs::path PathToScan, bool Recoursive);
	}
}
