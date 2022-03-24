//---------------------------------------------------------------------------
#ifndef DPathH
#define DPathH
//---------------------------------------------------------------------------
//#include "DCallback.hpp"
#include <chrono>
#include <functional>
#include "libdpp/DFilesystem.h"
#include "libdpp/DError.h"

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
						CALLBACK_DEC_BYTES=6,	// Trigger during Copy_File to send bytes left
						};

	// CanAccess() AccessRights
	#if defined _WIN32 || defined _WIN64
		static const unsigned long ACCESS_READ		=	GENERIC_READ;
		static const unsigned long ACCESS_WRITE		=	GENERIC_WRITE;
		static const unsigned long ACCESS_EXECUTE	=	GENERIC_EXECUTE;
		static const unsigned long ACCESS_ALL		=	GENERIC_ALL;
		std::string GetFileVersion(std::string Filename=std::string(), bool TrimDots = true);
        bool CanAccess(fs::path Path, DWORD AccessRights);
    #elif __linux__
        static const fs::perms ACCESS_READ      =   fs::perms::others_read;
        static const fs::perms ACCESS_WRITE     =   fs::perms::others_write;
        static const fs::perms ACCESS_EXECUTE   =   fs::perms::others_exec;
        static const fs::perms ACCESS_ALL       =   fs::perms::others_all;
        bool CanAccess(fs::path Path, fs::perms AccessRights);
	#endif

	// Callback
    typedef std::function<void (DProgressCode Cmd, long int Data)> DCallback;

    // Scan result
    typedef std::vector<fs::path> DPathList;

	fs::path GetExeFilename();
	std::string GetExt(fs::path Path);
	fs::path ChangeExt(fs::path Path, std::string NewExt, bool Execute);
	fs::path ChangeExt(fs::path Path, std::string NewExt, bool Execute, err::error_code &ec);
	bool IsOlderThanHrs(const fs::path& Path, const int Hrs);
	bool IsOlderThanMin(const fs::path& Path, const int Min);
	DError::DErrorCode IsOlderThanHrs(const fs::path& Path, const int Hrs, bool &Result);
	std::chrono::system_clock::time_point LastWriteTime(const fs::path Path);
	std::vector<std::string> ReadAllExts(fs::path Path);
    std::string GetPermissionsString(fs::path Path);
    std::string GetPermissionsString(fs::perms p);
	std::uintmax_t space_to_be_freed(const fs::path& dir, unsigned int percent_free_required);
	DError::DErrorCode Copy_File(const fs::path &From, const fs::path &To, bool OverwriteExisting, bool SafeMode = false);
	bool Copy_File_Posix(const char* SourceFile, const char* DestFile, bool OverwriteExisting, DCallback Callback = nullptr,size_t BufferSize = 0);
	//err::error_code CopyDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DGlobalCallback Callback);
	DError::DErrorCode CopyDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DCallback Callback = nullptr);
	//err::error_code MoveDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DGlobalCallback Callback);
	DError::DErrorCode MoveDir(fs::path SourceDir, fs::path DestDir, bool FailIfExists, DCallback Callback = nullptr);
	bool DeleteDir(fs::path Dir);
	int ListDirs(std::shared_ptr<DPathList> Result, fs::path PathToScan, bool Recursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll, bool& StopRequest);
	int ListDirs(std::shared_ptr<DPathList> Result, fs::path PathToScan, bool Recursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive, bool& StopRequest);
	int CountDirs(fs::path PathToScan, bool Recursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll, bool& StopRequest);
	int CountDirs(fs::path PathToScan, bool Recursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive, bool& StopRequest);
	int CountDirs(fs::path PathToScan, bool Recursive, bool& StopRequest);
	int ListFiles(std::shared_ptr<DPathList> Result, fs::path PathToScan, bool Recursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll, bool& StopRequest);
	int ListFiles(std::shared_ptr<DPathList> Result, fs::path PathToScan, bool Recursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive, bool& StopRequest);
	int ListFiles(std::shared_ptr<DPathList> Result, fs::path PathToScan, bool Recursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive);
	int CountFiles(fs::path PathToScan, bool Recursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll, bool& StopRequest);
	int CountFiles(fs::path PathToScan, bool Recursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive, bool& StopRequest);
	int CountFiles(fs::path PathToScan, bool Recursive, bool& StopRequest);
	int DeleteFiles(fs::path PathToSan, bool Recursive, std::vector<std::string> *NameContentList, bool NameWholeWord, std::vector<std::string> *ExtContentList, bool ExtWholeWord, bool CaseSensitive, bool FindAll, bool& StopRequest);
	int DeleteFiles(fs::path PathToScan, bool Recursive, std::string NameContent, bool NameWholeWord, std::string ExtContent, bool ExtWholeWord, bool CaseSensitive, bool& StopRequest);
	int DeleteFiles(fs::path PathToScan, bool Recursive, bool& StopRequest);
	int DeleteFiles(fs::path PathToScan, bool Recursive);
	bool Exists(fs::path Path);
	bool Exists_StdFs(fs::path Path);
	bool Exists_Posix(const char* const path);
	//bool IsDirectory(fs::path Path, err::error_code& ec);
	bool IsDirectory(fs::path Path);
} // DPath
} // DTools
#endif
