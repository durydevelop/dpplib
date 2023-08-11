#include <libdpp/DSyncFile.h>
#include <libdpp/DString.h>
#include <libdpp/DChrono.h>

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
	DSyncFile::DSyncFile(fs::path SourceFilename, fs::path DestFilename, bool SyncNow, bool SafeCopyMode) {
		Source=SourceFilename;
		Dest=DestFilename;
        if (DPath::Exists(Source)) {
			Ready=true;
		}
		else {
			LastStrStatus="Sync file "+Source.string()+" does not exist";
			Ready=false;
		}
		SafeMode=SafeCopyMode;
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
        if (!DPath::Exists(Source)) {
			if (!Ready) {
				// Already not ready
				LastSyncStatus=SYNC_NOT_READY;
			}
			else {
				LastStrStatus="Sync file "+Source.string()+" does not exist";
				Ready=false;
				LastSyncStatus=SYNC_ERR_FILE_NOT_FOUND;
			}
			return(LastSyncStatus);
		}

		if (Source == Dest) {
			LastStrStatus="Dest sync file is same as source";
			Ready=false;
			LastSyncStatus=SYNC_ERR_SAME_AS_DEST;
			return(LastSyncStatus);
		}

		if (!Ready) {
			// From !Ready to Ready, notify restored and real check next time
			Ready=true;
			LastSyncStatus=SYNC_RESTORED;
			return(LastSyncStatus);
		}

		Ready=true;
        LastStrStatus=Source.string()+" -> ";

        if (!DPath::Exists(Dest)) {
			LastStrStatus.append("not present, sync ");
		}
		else {
            std::chrono::system_clock::time_point st=DPath::LastWriteTime(Source);
            std::chrono::system_clock::time_point dt=DPath::LastWriteTime(Dest);
            std::string s=DChrono::FormatTimeP(st);
            std::string d=DChrono::FormatTimeP(dt);
			if (st > dt) {
				LastStrStatus.append("sync ");
			}
			else {
				LastStrStatus.append("no sync needed");
				LastSyncStatus=SYNC_NO_NEEDED;
				return(LastSyncStatus);
			}
		}

/*
        else if (DPath::LastWriteTime(Source) > DPath::LastWriteTime(Dest)) {
			LastStrStatus.append("sync ");
		}
		else {
			LastStrStatus.append("no sync needed");
			LastSyncStatus=SYNC_NO_NEEDED;
			return(LastSyncStatus);
		}
*/
		try {
            DError::DErrorCode ErrorCode=DPath::Copy_File(Source,Dest,true,SafeMode);
			if (ErrorCode.IsSet()) {
				LastStrStatus.append("error copying to "+Dest.string()+" : "+ErrorCode.Message());
				LastSyncStatus=SYNC_ERR_COPY;
				return(LastSyncStatus);
			}
			LastStrStatus.append("done");
			DSyncStatus ReturnSync=SYNC_DONE;
			if (LastSyncStatus < 0) { //== SYNC_ERROR) {
				ReturnSync=SYNC_RESTORED;
			}
			LastSyncStatus=SYNC_DONE;
            LastSyncTime=DChrono::FormatNow();
			return(ReturnSync);

		} catch (fs::filesystem_error& e) {
            LastStrStatus.append("sync error: "+std::string(e.what()));
            LastSyncStatus=SYNC_ERR_BHO;
		}
}
catch (std::exception& e) {
	LastStrStatus.append(std::string("Watcher DoSync() Exception: ")+e.what());
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
