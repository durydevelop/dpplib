#include "DUpdate.h"
#include "DString.h"
#include "DPath.h"

namespace fs=DTools::fs;
namespace err=DTools::err;
namespace pt=boost::property_tree;

// Repo info file content
#ifdef _WIN32
    #define FILE_INFO               "InfoWin.json"
#else
    #define FILE_INFO               "InfoNix.json"
#endif
#define SECTION_UPGRADE_INFO    "UpgradeInfo"
#define SECTION_FILES           "Files"
#define SECTION_REPLACE         "Replace"
#define SECTION_MODIFY          "Modify"
#define SECTION_LIST            "List"

#define SECTION_REPO            "Repo"
#define PARAM_REPO_TYPE         "RepoType"
#define PARAM_REPO_URI          "RepoUri"
#define PARAM_REPO_SUB_URI      "RepoSubUri"
#define PARAM_REPO_AUTH         "Authenticate"
#define PARAM_REPO_USER         "User"
#define PARAM_REPO_PWD          "Pwd"


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
        UpdateData=nullptr;
    }

    /**
     * @brief Desctructor
     */
    DUpdate::~DUpdate() {
        if (UpdateData != nullptr) {
            delete UpdateData;
        }
    }

    /**
     * @brief Populate repository data reading them from file.
     * @param Filename  ->  Json filename containing data.
     * @return true if loaded data are valid repository data.
     * json file must be structured as follow:
      {
        "Repo": {
                "RepoType": "FOLDER",
                "RepoUri": "\\\\smbserver\\path\\to\\folder",
                "RepoSubUri": "Update",
                "Authenticate": "false",
                "User": "",
                "Pwd": ""
        }
      }

      N.B. In *nix system, folder path can be
      "RepoUri": "/mnt/mounted/remote/folder"
     *
     */
    bool DUpdate::SetRepositoryFromFile(fs::path Filename) {
        Log("Updater: Load repo data from "+Filename.string());
        DTools::DPreferences RepoFile(Filename.string());
        if (!RepoFile.IsReady()) {
            Log("Updater: RepoInfoFile open error: "+RepoFile.GetLastStatus());
            return false;
        }

        SetRepository(RepoFile.ReadString(SECTION_REPO,PARAM_REPO_TYPE,""),
                      RepoFile.ReadString(SECTION_REPO,PARAM_REPO_URI,""),
                      RepoFile.ReadString(SECTION_REPO,PARAM_REPO_SUB_URI,""),
                      RepoFile.ReadBool(SECTION_REPO,PARAM_REPO_AUTH,false),
                      RepoFile.ReadString(SECTION_REPO,PARAM_REPO_USER,""),
                      RepoFile.ReadString(SECTION_REPO,PARAM_REPO_PWD,""));

        return (IsValidRepository());
    }

    /**
     * @brief Manually populate repository data
     * @param RepoType      ->  Must be REPO_TYPE_FOLDER, REPO_TYPE_HTTP ot REPO_TYPE_FTP string value.
     * @param RepoUri       ->  Folder path, http main url or ftp server.
     * @param RepoSubUri    ->  Can be empty in folder, can be second part of url in http/s, can be sub-folder in ftp.
     * @param Authenticate  ->  if true autentication is performed using RepoUser and RepoPwd using appropriate method for repo type
     * @param RepoUser      ->  User for authentication. If empty in ftp, "anonymous" is used.
     * @param RepoPwd       ->  Password for authentication (actually encryption is not suprted).
     */
    void DUpdate::SetRepository(std::string RepoType,std::string RepoUri, std::string RepoSubUri, bool Authenticate, std::string RepoUser, std::string RepoPwd) {
        dRepository.RepoType=RepoType;
        dRepository.MainUri=RepoUri;
        dRepository.SubUri=RepoSubUri;
        dRepository.NeedAuth=Authenticate;
        dRepository.User=RepoUser;
        dRepository.Password=RepoPwd;
    }

    /**
     * @return true if current DRepository is valid, otherwise false;
     */
    bool DUpdate::IsValidRepository(void) {
        if (dRepository.MainUri.empty()) {
            Log("Updater: Repo Uri "+dRepository.MainUri+" not valid");
            return false;
        }

        if (dRepository.RepoType == REPO_TYPE_FOLDER) {
            if (dRepository.NeedAuth) {
                // TODO
            }
            return true;
        }
        else if (dRepository.RepoType == REPO_TYPE_HTTP) {
            // TODO
            return true;
        }
        else if (dRepository.RepoType == REPO_TYPE_FTP) {
            // TODO
            return true;
        }
        Log("Updater: Repo type "+dRepository.RepoType+" not valid");
        return false;
    }

    /**
     * @brief Execute all needed operation for Update.
     * @return true on succeed otherwise false (use GelLastStatus() to retrive error text).
     */
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
     * @return true on succeed otherwise false (use GelLastStatus() to retrive error text).
     */
    bool DUpdate::DownloadRemoteInfoFile(void) {
        if (!Ready) return false;
        fs::path RemoteInfoFilename=fs::path(dRepository.MainUri) / dRepository.SubUri / FILE_INFO;
        if (dRepository.RepoType == REPO_TYPE_FOLDER) {
            // Copy from remote network folder
            if (dRepository.NeedAuth) {
                // TODO
            }
            std::string LogMsg="copy repo file"+RemoteInfoFilename.string()+" to "+LocalInfoFilename.string();
            err::error_code ec=DTools::DPath::Copy_File(RemoteInfoFilename,LocalInfoFilename,true,true);
            if (ec.value() != 0) {
                Log("Updater error: "+LogMsg+" : "+ec.message());
                Ready=false;
                return false;
            }
            else {
                Log("Updater done: "+LogMsg);
            }
        }
        else if (dRepository.RepoType == REPO_TYPE_HTTP) {
            // TODO
        }
        else if (dRepository.RepoType == REPO_TYPE_FTP) {
            // TODO
        }
        else {
            Log("Updater: repo type unkown????");
            Ready=false;
            return false;
        }
        return true;
    }

    /**
     * @brief Parse repo info file, check app name matching and upgrade version.
     * @return true on succeed otherwise false (use GelLastStatus() to retrive error text).
     */
    bool DUpdate::ParseRepoInfoFile(void) {
        if (!Ready) return false;

        if (UpdateData == nullptr) {
            UpdateData=new DTools::DPreferences(LocalInfoFilename.string());
            if (!UpdateData->IsReady()) {
                Log("Update info file parse error: "+UpdateData->GetLastStatus());
                return false;
            }
        }

        // Check app name
        std::string RepoAppName=UpdateData->ReadString(SECTION_UPGRADE_INFO,PARAM_APP_NAME,"");
        if (RepoAppName.empty() || RepoAppName != CurrAppName) {
            Log("Update AppName does't match");
            return false;
        }

        // Read Version
        std::string RepoVersionStr=UpdateData->ReadString(SECTION_UPGRADE_INFO,PARAM_UPGRADE_VERSION,"");
        int RepoVersionNr=DTools::DString::ToNumber<int>(RepoVersionStr);
        if (RepoVersionNr > CurrVersionNr) {
            Log("Update "+RepoVersionStr+" available");
        }

        return true;
    }

    /**
     * @brief Download all files needed by ugrade. Filenames are store in SECTION_REPLACE and SECTION_UPDATE section in info file.
     * @return true on succeed otherwise false (use GelLastStatus() to retrive error text).
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
            UpdateData->ReadItemNames(NodeName,Files);
            for (std::string Source : Files) {
                // Make filenames
                fs::path SourceFilename=fs::path(dRepository.MainUri) / dRepository.SubUri / Source;
                fs::path DestFilename=UpdateTempDir / Source; // same as repo
                if (!fs::exists(SourceFilename)) {
                    Log("Updater: file missing: "+SourceFilename.string());
                    return false;
                }

                // copy to local temp
                std::string LogMsg="copy "+SourceFilename.string()+" to "+DestFilename.string();
                bool ret=DTools::DPath::Copy_File(SourceFilename.string().c_str(),DestFilename.string().c_str(),0);
                if (!ret) {
                    Log("Updater: error "+LogMsg);
                    Ready=false;
                    return false;
                }
                else {
                    Log("Updater: done copy "+LogMsg);
                }
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
            Log("Updater: repo type unkown????");
            Ready=false;
            return false;
        }
        return true;
    }

    /**
     * @brief Apply download update files: backup files to udate, than replace them or modify them (as written in update file).
     * @return true on succeed otherwise false (use GelLastStatus() to retrive error text).
     */
    bool DUpdate::ApplyUpdate(void) {
        if (!Ready) return false;
        // Read files list
        std::vector<std::string> Files;
        std::string NodeName=SECTION_FILES "." SECTION_REPLACE;
        UpdateData->ReadItemNames(NodeName,Files);
        for (std::string Source : Files) {
            // Real file name
            std::string RealName=UpdateData->ReadDotString(NodeName,Source,"");
            // Source filename (downloaded one)
            fs::path SourceFilename=UpdateTempDir / Source;
            if (!fs::exists(SourceFilename)) {
                Log("Updater: previous downloaded file missing: "+Source);
                return false;
            }
            // Real dest filename
            fs::path DestFilename=fs::current_path() / RealName;

            // Backup if already exists
            std::string LogMsg;
            if (fs::exists(DestFilename)) {
                // Backup dest filename
                fs::path BakFilename=BackupDir / RealName;
                LogMsg="backup "+DestFilename.string()+" to "+BakFilename.string();
                //err::error_code ec=DTools::DPath::Copy_File(DestFilename,BakFilename,true,true);
                err::error_code ec;
                fs::rename(DestFilename,BakFilename,ec);
                if (ec.value() != 0) {
                    Log("Updater: error "+LogMsg+" : "+ec.message());
                    Ready=false;
                    return false;
                }
                Log("Updater: done "+LogMsg);
            }

            // Replace with new one
            LogMsg="installing file "+SourceFilename.string()+" to "+DestFilename.string();
            //err::error_code ec=DTools::DPath::Copy_File(UpdFilename,DestFilename,true);
            err::error_code ec;
            fs::rename(SourceFilename,DestFilename,ec);
            if (ec.value() != 0) {
                Log("Updater: error "+LogMsg+" : "+ec.message());
                Ready=false;
                return false;
            }
            Log("Updater: done "+LogMsg);
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
	 * @brief Perform the callback.
	 * @param Msg	->	Log message to send.
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
