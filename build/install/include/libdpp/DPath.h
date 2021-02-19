//---------------------------------------------------------------------------
#ifndef DPathH
#define DPathH
//---------------------------------------------------------------------------
//#include "DCallback.hpp"
#include <chrono>
#include <functional>
#include "DFilesystem.h"

// Macro ridefined from basestd.h
#define DIntToPtr(i) (void *)(uintptr_t)(i)

namespace DTools
{
	namespace DPath {
	enum DProgressCode {INC_FILE=1,     // Trigger when a file is copied or moved
						INC_DIR=2,      // Trigger when a file is copied or moved
						SET_FILES=3,    // Trigger to send nr of files to copy or move
						SET_DIRS=4,     // Trigger to send nr of dirs to copy or move
						SET_OBJ=5,      // Trigger to send nr of dirs+file to copy or move
						DEC_BYTES=6		// Trigger during Copy_File to send bytes left
						};
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
		err::error_code Copy_File(const fs::path &From, const fs::path &To, bool OverwriteExisting, bool SafeMode = false);
		bool Copy_File(const char* SourceFile, const char* DestFile, bool OverwriteExisting, DMemberCallback MemberCallback = nullptr, void *MemberCallbackClass = nullptr, size_t BufferSize = 0);
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
