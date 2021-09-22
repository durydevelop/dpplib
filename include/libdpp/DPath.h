//---------------------------------------------------------------------------
#ifndef DPathH
#define DPathH
//---------------------------------------------------------------------------
//#include "DCallback.hpp"
#include <chrono>
#include <functional>
#include "libdpp/DFilesystem.h"

#ifdef _WIN32
    #include <windows.h>    //GetModuleFileNameW
    //#include <winbase.h>    //GetFileSecurity
#else
    #include <limits.h>
    #include <unistd.h>     //readlink
#endif

namespace DTools
{
namespace DPath
{
	enum DProgressCode {CALLBACK_INC_FILE=1,    // Trigger when a file is copied or moved
						CALLBACK_INC_DIR=2,     // Trigger when a dir is copied or moved
						CALLBACK_SET_FILES=3,   // Trigger to send nr of files to copy or move
						CALLBACK_SET_DIRS=4,    // Trigger to send nr of dirs to copy or move
						CALLBACK_SET_OBJS=5,    // Trigger to send nr of dirs+file to copy or move
						CALLBACK_DEC_BYTES=6	// Trigger during Copy_File to send bytes left
						};
	#if defined _WIN32 || defined _WIN64
		static const DWORD ACCESS_READ		=	GENERIC_READ;
		static const DWORD ACCESS_WRITE		=	GENERIC_WRITE;
		static const DWORD ACCESS_EXECUTE	=	GENERIC_EXECUTE;
		static const DWORD ACCESS_ALL		=	GENERIC_ALL;
/*
        enum DAccessRights {ACCESS_READ    =    GENERIC_READ,
                            ACCESS_WRITE   =    GENERIC_WRITE,
                            ACCESS_EXECUTE =    GENERIC_EXECUTE,
                            ACCESS_ALL     =    GENERIC_ALL
        };
*/
		std::string GetFileVersion(std::string Filename=std::string(), bool TrimDots = true);

    #elif __linux__
        static const DWORD ACCESS_READ      =   fs::perms::others_read;
        static const DWORD ACCESS_WRITE     =   fs::perms::others_write;
        static const DWORD ACCESS_EXECUTE   =   fs::perms::others_exec;
        static const DWORD ACCESS_ALL       =   fs::perms::others_all;
/*
        enum DAccessRights {ACCESS_READ    =   (unsigned) fs::perms::others_read,
                            ACCESS_WRITE   =   (unsigned) fs::perms::others_write,
                            ACCESS_EXECUTE =   (unsigned) fs::perms::others_exec,
                            ACCESS_ALL     =   (unsigned) fs::perms::others_all
*/
    };
	#endif
	bool CanAccess(fs::path Path, DWORD AccessRights);

	// Callback
	typedef std::function<void (DProgressCode Cmd, long int Data)> DCallback;

	fs::path GetExeFilename();
	fs::path ChangeExt(fs::path Path, std::string NewExt, bool Execute, err::error_code *ec = NULL);
	bool IsOlderThanHrs(const fs::path& Path, const int Hrs);
	std::chrono::system_clock::time_point LastWriteTime(const fs::path Path);
	std::vector<std::string> ReadAllExts(fs::path Path);
    std::string GetPermissionsString(fs::path Path);
    std::string GetPermissionsString(fs::perms p);
	std::uintmax_t space_to_be_freed(const fs::path& dir, unsigned int percent_free_required);
	err::error_code Copy_File(const fs::path &From, const fs::path &To, bool OverwriteExisting, bool SafeMode = false);
	bool Copy_File_Posix(const char* SourceFile, const char* DestFile, bool OverwriteExisting, DCallback Callback = nullptr,size_t BufferSize = 0);
	//err::error_code CopyDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DGlobalCallback Callback);
	err::error_code CopyDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DCallback Callback = nullptr);
	//err::error_code MoveDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DGlobalCallback Callback);
	err::error_code MoveDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DCallback Callback = nullptr);
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
	int DeleteFiles(fs::path PathToSan, bool Recoursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll);
	int DeleteFiles(fs::path PathToScan, bool Recoursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive);
	int DeleteFiles(fs::path PathToScan, bool Recoursive);
	bool Exists(fs::path Path);
} // DPath
} // DTools
#endif
