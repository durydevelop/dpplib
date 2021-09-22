#include "libdpp/DSyncFile.h"
#include "libdpp/DString.h"

namespace DTools
{
	/**
	* @class DSyncFile
	*
	* @brief C++ std class for synchronise files
	*
	* @author $Author
	*
	* \mainpage DSyncFile class
	*
	* \section intro_sec Intro
	*
	*
	*
	* \section futures_sec Futures
	**/

	/**
	 * @brief DSyncFile::DSyncFile
	 * @param SourceFilename
	 * @param DestFilename
	 * @param SyncNow
	 */
	DSyncFile::DSyncFile(fs::path SourceFilename, fs::path DestFilename, bool SyncNow) {
		Source=SourceFilename;
		Dest=DestFilename;
		Ready=false;
		SafeMode=false;
		if (SyncNow) {
			DoSync();
		}
		else {
			LastSyncStatus=SYNC_NOT_YET;
            //LastStrStatus=Source.string()+" sync on creation not required";
		}
	}

	DSyncFile::DSyncStatus DSyncFile::DoSync(void) {
try {
		if (!fs::exists(Source)) {
			LastStrStatus="Sync file "+Source.string()+" does not exist";
			Ready=false;
			LastSyncStatus=SYNC_ERROR;
			return(LastSyncStatus);
		}

		if (Source == Dest) {
			LastStrStatus="Dest sync file is same as source";
			Ready=false;
			LastSyncStatus=SYNC_ERROR;
			return(LastSyncStatus);
		}

		Ready=true;
        LastStrStatus=Source.string()+" -> ";

		if (!fs::exists(Dest)) {
			LastStrStatus.append("not present, sync ");
		}
		else if (DTools::DPath::LastWriteTime(Source) > DTools::DPath::LastWriteTime(Dest)) {
			LastStrStatus.append("sync ");
		}
		else {
			LastStrStatus.append("no sync needed");
			LastSyncStatus=SYNC_NO_NEEDED;
			return(LastSyncStatus);
		}

		try {
			DTools::err::error_code ec=DTools::DPath::Copy_File(Source,Dest,true,SafeMode);
			if (ec.value() != 0) {
				LastStrStatus.append("error copying to "+Dest.string()+" : "+ec.message());
				LastSyncStatus=SYNC_ERROR;
				return(LastSyncStatus);
			}
			LastStrStatus.append("done");
			LastSyncStatus=SYNC_DONE;
			LastSyncTime=DTools::DString::FormatNow();

		} catch (fs::filesystem_error& e) {
            LastStrStatus.append("sync error: "+std::string(e.what()));
			LastSyncStatus=SYNC_ERROR;
		}
}
catch (std::exception& e) {
	LastStrStatus.append(std::string("Watcher Exception: ")+e.what());
}
		return(LastSyncStatus);
	}

	bool DSyncFile::IsReady(void) {
		return(Ready);
	}

	void DSyncFile::InvertSync(void) {
		fs::path Temp=Source;
		Source=Dest;
		Dest=Temp;
	}

	void DSyncFile::SetSafeMode(bool Enabled) {
		SafeMode=Enabled;
	}

	bool DSyncFile::GetSafeMode(void) {
		return(SafeMode);
	}
}
