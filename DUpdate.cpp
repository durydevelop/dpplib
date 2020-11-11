#include "DUpdate.h"
#include "DString.h"
#include "DPath.h"

namespace fs=DTools::fs;
namespace err=DTools::err;
namespace pt=boost::property_tree;

// Repo info file content
#define FILE_INFO               "Info.json"
#define SECTION_UPGRADE_INFO    "UpgradeInfo"
#define SECTION_FILES           "Files"
#define SECTION_REPLACE         "Replace"
#define SECTION_MODIFY          "Modify"
#define SECTION_LIST            "List"

#define PARAM_APP_NAME          "AppName"
#define PARAM_UPGRADE_VERSION   "Version"
#define PARAM_CASE_SENSITIVE    "CaseSensitive"

namespace DTools {
    /**
     * @brief Contructor
     * @param ApplicationName   ->  Name used to ensure to use right update
     * @param CurrentVersion    ->  String containing current app version, may be something like "1.0.0.2" all the non digit characters will be trimmed and the string transformed to number
     */
    DUpdate::DUpdate(std::string ApplicationName, std::string CurrentVersion) {
        // UpdateTempDir
        UpdateTempDir=fs::current_path() / "Update";
        if (!fs::exists(UpdateTempDir)) {
            err::error_code ec;
            if (fs::create_directory(UpdateTempDir,ec)) {
                Ready=true;
                Log("Update dir created");
            }
            else {
                Ready=false;
                Log("Update dir creating error: "+ec.message());
            }
        }
        else {
            Ready=true;
        }

        // Backupdir
        BackupDir=fs::current_path() / "Backup";
        if (!fs::exists(BackupDir)) {
            err::error_code ec;
            if (fs::create_directory(BackupDir,ec)) {
                Ready=true;
                Log("Updater: backup dir created");
            }
            else {
                Ready=false;
                Log("Updater: backup dir creating error: "+ec.message());
            }
        }
        else {
            Ready=true;
        }

        LocalInfoFilename=UpdateTempDir / FILE_INFO;

        CurrAppName=ApplicationName;
        if (CurrAppName.empty()) {
            Ready=false;
        }

        CurrVersionNr=DString::ToNumber<int>(CurrentVersion);
        RepoInfo=nullptr;
    }

    DUpdate::~DUpdate() {
        if (RepoInfo != nullptr) {
            delete RepoInfo;
        }
    }

    /**
     * @brief Populate repository data
     * @param RepoType
     * @param RepoUri
     * @param RepoSubUri
     * @param Authenticate
     * @param RepoUser
     * @param RepoPwd
     */
    void DUpdate::SetRepository(DRepoType RepoType,std::string RepoUri, std::string RepoSubUri, bool Authenticate, std::string RepoUser, std::string RepoPwd) {
        dRepository.RepoType=RepoType;
        dRepository.MainUri=RepoUri;
        dRepository.SubUri=RepoSubUri;
        dRepository.NeedAuth=Authenticate;
        dRepository.User=RepoUser;
        dRepository.Password=RepoPwd;
    }

    bool DUpdate::DoUpgrade(void) {
        if (!DownloadRemoteInfoFile()) {
            return false;
        }

        if (!ParseRepoInfoFile()) {
            return false;
        }

        if (!DownloadFiles()) {
            return false;
        }

        return(ApplyUpdate());
    }

    /**
     * @brief Download repo info file from remote repository.
     * @return true on succesfully copy to UpdateTempDir, otherwise false.
     */
    bool DUpdate::DownloadRemoteInfoFile(void) {
        if (!Ready) return false;
        fs::path RemoteInfoFilename=fs::path(dRepository.MainUri) / dRepository.SubUri / FILE_INFO;
        if (dRepository.RepoType == REPO_TYPE_FOLDER) {
            // Copy from remote network folder
            if (dRepository.NeedAuth) {
                // TODO
            }
            Log("Updater: Copy "+RemoteInfoFilename.string()+" to "+LocalInfoFilename.string());
            err::error_code ec=DTools::DPath::Copy_File(RemoteInfoFilename,LocalInfoFilename,true,true);
            if (ec.value() != 0) {
                Log("Updater: info file copy error: "+ec.message());
                Ready=false;
                return false;
            }
            else {
                Log("Updater: done: "+ec.message());
            }
        }
        else if (dRepository.RepoType == REPO_TYPE_HTTP) {
            // TODO
        }
        else if (dRepository.RepoType == REPO_TYPE_FTP) {
            // TODO
        }
        else {
            Log("Repo type unkown????");
            Ready=false;
            return false;
        }
        return true;
    }

    /**
     * @brief Parse repo info file, check app name matching and upgrade version.
     * @return true if app name match, false if any parsing errors occours.
     * After return, UpgradeAvailable is set to true if upgrade version is major that current version
     */
    bool DUpdate::ParseRepoInfoFile(void) {
        if (!Ready) return false;

        if (RepoInfo == nullptr) {
            RepoInfo=new DTools::DPreferences::DPreferences(LocalInfoFilename.string());
            if (!RepoInfo->IsReady()) {
                Log("Update info file parse error: "+RepoInfo->GetLastStatus());
                return false;
            }
        }

        // Check app name
        std::string RepoAppName=RepoInfo->ReadString(SECTION_UPGRADE_INFO,PARAM_APP_NAME,"");
        if (RepoAppName.empty() || RepoAppName != CurrAppName) {
            Log("Update AppName does't match");
            return false;
        }

        // Read Version
        std::string RepoVersionStr=RepoInfo->ReadString(SECTION_UPGRADE_INFO,PARAM_UPGRADE_VERSION,"");
        int RepoVersionNr=DTools::DString::ToNumber<int>(RepoVersionStr);
        if (RepoVersionNr > CurrVersionNr) {
            Log("Update "+RepoVersionStr+" available");
        }

        return true;
    }

    /**
     * @brief Download all files needed by ugrade. Filenames are store in SECTION_REPLACE and SECTION_UPDATE section in info file
     * @return true if all files has been downloaded, otherwise false;
     */
    bool DUpdate::DownloadFiles(void) {
        if (!Ready) return false;
        fs::path RemoteInfoFilename=fs::path(dRepository.MainUri) / dRepository.SubUri / FILE_INFO;
        if (dRepository.RepoType == REPO_TYPE_FOLDER) {
            // Copy from remote network folder
            if (dRepository.NeedAuth) {
                // TODO
            }

            // Read replace files list
            std::vector<std::string> Files;
            std::string NodeName=SECTION_FILES "." SECTION_REPLACE;
            RepoInfo->ReadItemNames(NodeName,Files);
            for (std::string Source : Files) {
                // Make filenames
                fs::path SourceFilename=fs::path(dRepository.MainUri) / dRepository.SubUri / Source;
                fs::path DestFilename=UpdateTempDir / Source; // same as repo
                if (!fs::exists(SourceFilename)) {
                    Log("Updater: file missing: "+SourceFilename.string());
                    return false;
                }
                // copy to local temp
                Log("Updater: Copy "+SourceFilename.string()+" to "+DestFilename.string());

                //err::error_code ec=DTools::DPath::Copy_File(SourceFilename,DestFilename,true,true);
                bool ret=fs::copy_file(SourceFilename,DestFilename);
                //if (ec.value() != 0) {
                if (!ret) {
                    //Log("Updater: download file "+SourceFilename.string()+" error: "+ec.message());
                    Ready=false;
                    return false;
                }
                else {
                    //Log("Updater: done: "+ec.message());
                }

                //Copy_File(SourceFilename.string().c_str(),DestFilename.string().c_str());
            }
            return true;
        }
        else if (dRepository.RepoType == REPO_TYPE_HTTP) {
            // TODO
        }
        else if (dRepository.RepoType == REPO_TYPE_FTP) {
            // TODO
        }
        else {
            Log("Repo type unkown????");
            Ready=false;
            return false;
        }
        return true;
    }

    bool DUpdate::ApplyUpdate(void) {
        if (!Ready) return false;
        // Read files list
        std::vector<std::string> Files;
        std::string NodeName=SECTION_FILES "." SECTION_REPLACE;
        RepoInfo->ReadItemNames(NodeName,Files);
        for (std::string Source : Files) {
            // Real file name
            std::string RealName=RepoInfo->ReadDotString(NodeName,Source,"");
            // Source filename (downloaded one)
            fs::path SourceFilename=UpdateTempDir / Source;
            if (!fs::exists(SourceFilename)) {
                Log("Updater: previous downloaded file missing: "+Source);
                return false;
            }
            // Real dest filename
            fs::path DestFilename=fs::current_path() / RealName;

            // Backup if already exists
            if (fs::exists(DestFilename)) {
                // Backup dest filename
                fs::path BakFilename=BackupDir / RealName;
                Log("Updater: Backup "+DestFilename.string()+" to "+BakFilename.string());
                //err::error_code ec=DTools::DPath::Copy_File(DestFilename,BakFilename,true,true);
                err::error_code ec;
                fs::rename(DestFilename,BakFilename,ec);
                if (ec.value() != 0) {
                    Log("Updater: backuping file "+RealName+" error: "+ec.message());
                    Ready=false;
                    return false;
                }
            }

            // Replace with new one
            Log("Updater: installing file "+SourceFilename.string()+" to "+DestFilename.string());
            //err::error_code ec=DTools::DPath::Copy_File(UpdFilename,DestFilename,true);
            err::error_code ec;
            fs::rename(SourceFilename,DestFilename,ec);
            if (ec.value() != 0) {
                Log("Updater: Installing file "+RealName+" error: "+ec.message());
                Ready=false;
                return false;
            }
        }
        return true;
    }




    // ******************************  Callback stuffs ***************************************
    /**
     * @brief Register a global callback.
     * @param callback  ->  DGlobalCallback type function to register.
     */
    void DUpdate::SetGlobalCallback(DGlobalCallback callback)	{
            GlobalCallback=callback;
            MemberCallback=nullptr;
            MemberCalbackObj=nullptr;
    }

	/**
	 * @brief Register a class member callback.
	 * @param callback  ->  DMemberCallback type function to register (e.g. ClassName::CallbackFunc).
	 * @param ClassObj  ->  Class pointer in which callback is called.
	 */
	void DUpdate::SetMemberCallback(DMemberCallback callback, void *ClassObj) {
			GlobalCallback=nullptr;
			MemberCallback=callback;
			MemberCalbackObj=ClassObj;
	}

	/**
	 * @brief Perform the callback
	 * @param Path			->	Filename that has been changed.
	 * @param SyncStatus	->	Detected Change type. Can be CHANGE_STATUS_CREATED, CHANGE_STATUS_ERASED, CHANGE_STATUS_MODIFIED.
	 */
	void DUpdate::DoCallback(std::string Msg) {
		if(GlobalCallback != NULL) {
			GlobalCallback(Msg);
		}
		else if((MemberCallback != NULL) && (MemberCalbackObj != NULL)) {
			MemberCallback(MemberCalbackObj,Msg);
		}
	}
	// ***************************************************************************************

	// *******************************  Log Functions ****************************************
	/**
	* @brief Set LastStrStatus and make log callback.
	* @param LogMsg	->  Message to log.
	*
	* N.B. If LogMsg is empty, callback with LastStrStatus is performed.
	**/
	void DUpdate::Log(std::string LogMsg) {
		if (!LogMsg.empty()) {
			LastStrStatus=LogMsg;
		}
		DoCallback(LastStrStatus);
	}

	//! @return LastStrStatus string.
	std::string DUpdate::GetLastStatus(void) {
		return(LastStrStatus);
	}
	// ***************************************************************************************
}
