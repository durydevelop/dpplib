#include "DUpdate.h"
#include "DString.h"
#include "DPath.h"

namespace fs=DTools::fs;
namespace err=DTools::err;
namespace pt=boost::property_tree;

#define FILE_INFO               "Info.json"
#define SECTION_INFO            "UpgradeInfo"
#define SECTION_REPLACE         "ReplaceFiles"
#define PARAM_APP_NAME          "AppName"
#define PARAM_UPGRADE_VERSION   "Version"

namespace DTools {
    /**
     * @brief Contructor
     * @param ApplicationName   ->  Name used to ensure to use right update
     * @param CurrentVersion    ->  String containing current app version, may be something like "1.0.0.2" all the non digit characters will be trimmed and the string transformed to number
     */
    DUpdate::DUpdate(std::string ApplicationName, std::string CurrentVersion) {
        UpdateTempDir=fs::current_path() / "Update";
        // create Updatedir if not exists
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

        LocalInfoFilename=UpdateTempDir / FILE_INFO;

        CurrAppName=ApplicationName;
        if (CurrAppName.empty()) {
            Ready=false;
        }

        CurrVersionNr=DString::ToNumber<int>(CurrentVersion);
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

    /**
     * @brief Get info file from remote repository
     * @return true if file is succesfully copied to UpdateTempDir
     */
    bool DUpdate::DownloadRemoteInfoFile(void) {
        if (!Ready) return false;
        fs::path RemoteInfoFilename=fs::path(dRepository.MainUri) / dRepository.SubUri / FILE_INFO;
        if (dRepository.RepoType == REPO_TYPE_FOLDER) {
            // Copy from remote network folder
            if (dRepository.NeedAuth) {
                // TODO
            }
            err::error_code ec=DTools::DPath::Copy_File(RemoteInfoFilename,LocalInfoFilename,true);
            if (ec.value() != 0) {
                Log("Update info file copy error: "+ec.message());
                Ready=false;
                return false;
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
     * @brief DUpdate::ParseRepoInfoFile
     * @return
     */
    bool DUpdate::ParseRepoInfoFile(void) {
        if (!Ready) return false;
        UpgradeAvailable=false;

        RepoInfo=new DTools::DPreferences::DPreferences(LocalInfoFilename.string());
        if (!RepoInfo->IsReady()) {
            Log("Update info file parse error: "+RepoInfo->GetLastStatus());
            return false;
        }

        // Check app name
        std::string RepoAppName=RepoInfo.get<std::string>(SECTION_INFO "." PARAM_APP_NAME,"");
        if (RepoAppName != CurrAppName) {
            Log("Update AppName does't match");
            return false;
        }

        // Read Version
        std::string RepoVersionStr=RepoInfo.get<std::string>(SECTION_INFO "." PARAM_UPGRADE_VERSION,"");
        int RepoVersionNr=DTools::DString::ToNumber<int>(RepoVersionStr);
        if (RepoVersionNr > CurrVersionNr) {
            Log("Update "+RepoVersionStr+" available");
            UpgradeAvailable=true;
        }

        return true;
    }

    bool DUpdate::DownloadFiles(void) {
        if (!Ready) return false;
        fs::path RemoteInfoFilename=fs::path(dRepository.MainUri) / dRepository.SubUri / FILE_INFO;
        if (dRepository.RepoType == REPO_TYPE_FOLDER) {
            // Copy from remote network folder
            if (dRepository.NeedAuth) {
                // TODO
            }

            // Read files list

            err::error_code ec=DTools::DPath::Copy_File(RemoteInfoFilename,LocalInfoFilename,true);
            if (ec.value() != 0) {
                Log("Update info file copy error: "+ec.message());
                Ready=false;
                return false;
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
