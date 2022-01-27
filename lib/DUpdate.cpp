#include <libdpp/DUpdate.h>
#include <libdpp/DString.h>
#include <libdpp/DPath.h>
#include <libdpp/DShell.h>
#include <libdpp/qt/DQt.h>
#include <fstream>

namespace fs=DTools::fs;
namespace err=DTools::err;
namespace pt=boost::property_tree;

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
#define PARAM_MAIN_EXE          "MainExe"

// File in remote repository containing update rules.
#define FILENAME_REPO_RULES     "UpdateRules.json"
// Local file containing repo location data (if exists in root path of executable, it will be used to setup repository in class constructor).
#define FILENAME_REPO_DATA      "UpdateRepo.json"
// Filename used to execute update process.
#ifdef _WIN32
    #define FILENAME_UPDATER_EXE    "DuryUpdater.exe"
#else
    #define FILENAME_UPDATER_EXE    "DuryUpdater"
#endif
// Used to avoid unwanted loop if newst update make some issues.
#define FILENAME_JUST_UPDATE    ".justupdate"

/**
* @class DLog
*
* @brief C++ std class to manage program upgrade.
*
* @author $Author
*
* \mainpage DUpdate class
*
* \section intro_sec Intro
*
* \section todo_sec TODO
* -Send files to repo
* -Delete update files
*
*
* \section futures_sec How to use:
* 1_Create UpdateRepo.json as here:
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
    2_Put file in program folder
    3_create UpdateRules.json as here:
    {
        "UpgradeInfo": {
            "AppName": "DuryFinder",
            "Version": "0.2.0.2",
            "MainExe": "DuryFinder.exe"
        },
        "Files": {
            "Replace": {
                "DuryFinder.agg": "DuryFinder.exe"
            },
            "Modify": {
                "test.txt": {
                    "CaseSensitive": "true",
                    "List": {
                        "Text1 to replace": "Replaced text1",
                        "Text2 to replace": "Replaced text2"
                    }
                }
            }
        }
    }
    "AppName" must be the same as ApplicationName string used in contructor
    "Version" in this file must corresponds to "MainExe" version in reposiroty
    4_Add source code to your app:
    @code
    void MainWindow::CheckForUpdate(void) {
        std::string Version=FILE_VERSION_STR;
        DTools::DUpdate dUpdate(PROGRAM_NAME,Version,MainWindow::UpdaterCallback,this);
        //dUpdate.SetMemberCallback(MainWindow::UpdaterCallback,this); // use this api to set callback not in contructor
        //if (!dUpdate.SetRepositoryFromFile(DTools::fs::current_path() / "repo.json")) return; // Use this api to manual set file with repository info (default "UpdateRepo.json")
        if (dUpdate.IsValidRepository()) {
            dUpdate.DoUpgrade(); // All in one upgrade
        }
    }
    @endcode
**/

namespace DTools
{
    /**
     * @brief Contructor
     * @param ApplicationName   ->  Name used to ensure to use right update
     * @param CurrentVersion    ->  String containing current app version, may be something like "1.0.0.2" all the non digit characters will be trimmed and the string transformed to number
     */
    DUpdate::DUpdate(const std::string ApplicationName, const std::string CurrentVersion) {
        // Callbacks
        MemberCallback=nullptr;

        Init(ApplicationName,CurrentVersion);
    }

    /**
     * @brief Desctructor
     */
    DUpdate::~DUpdate() {
        if (UpdateData != nullptr) {
            delete UpdateData;
        }
    }

    void DUpdate::Init(const std::string ApplicationName, const std::string CurrentVersion) {
        // UpdateTempDir
        UpdateTempDir=fs::current_path() / "Update";
        if (!DTools::DPath::Exists(UpdateTempDir)) {
            err::error_code ec;
            if (fs::create_directory(UpdateTempDir,ec)) {
                Ready=true;
                Log("Updater: update dir created");
            }
            else {
                Ready=false;
                Log("Updater: update dir creating error: "+ec.message());
            }
        }
        else {
            Ready=true;
        }

        // Backupdir
        BackupDir=fs::current_path() / "Backup";
        if (!DTools::DPath::Exists(BackupDir)) {
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

        // Local rules filename
        LocalInfoFilename=UpdateTempDir / FILENAME_REPO_RULES;

        // App name
        CurrAppName=ApplicationName;
        if (CurrAppName.empty()) {
            Log("Updater: App name is empty");
            Ready=false;
        }

        // App version
        CurrVersionNr=DString::ToNumber<int>(DString::RemoveNotDigitCopy(CurrentVersion));
        UpdateData=nullptr;
        UpdateNeeded=false;

        // Populate repo if repo info file is present
        if (DTools::DPath::Exists(DTools::fs::current_path() / FILENAME_REPO_DATA)) {
            Ready=SetRepositoryFromFile(DTools::fs::current_path() / FILENAME_REPO_DATA);
        }


        CurrExeName=DPath::GetExeFilename();
        //Log("Exe name "+CurrExeName.stem().string());
        AsUpdater=false;
        #ifdef _WIN32
            AsUpdater=DTools::DString::CmpNoCase(CurrExeName.filename().string(),FILENAME_UPDATER_EXE);
        #else
            AsUpdater=CurrExeName.stem().string() == FILENAME_UPDATER_EXE;
        #endif
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
        Log("Updater: load repo data from "+Filename.string());
        DTools::DPreferences RepoFile(Filename.string());
        if (!RepoFile.IsReady()) {
            Log("Updater: repoInfoFile open error: "+RepoFile.GetLastStatus());
            return false;
        }

        Ready=SetRepository(RepoFile.ReadString(SECTION_REPO,PARAM_REPO_TYPE,""),
                            RepoFile.ReadString(SECTION_REPO,PARAM_REPO_URI,""),
                            RepoFile.ReadString(SECTION_REPO,PARAM_REPO_SUB_URI,""),
                            RepoFile.ReadBool(SECTION_REPO,PARAM_REPO_AUTH,false),
                            RepoFile.ReadString(SECTION_REPO,PARAM_REPO_USER,""),
                            RepoFile.ReadString(SECTION_REPO,PARAM_REPO_PWD,""));

        return(Ready);
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
    bool DUpdate::SetRepository(std::string RepoType,std::string RepoUri, std::string RepoSubUri, bool Authenticate, std::string RepoUser, std::string RepoPwd) {
        dRepository.RepoType=RepoType;
        dRepository.MainUri=RepoUri;
        dRepository.SubUri=RepoSubUri;
        dRepository.NeedAuth=Authenticate;
        dRepository.User=RepoUser;
        dRepository.Password=RepoPwd;

        Ready=IsValidRepository();

        if (Ready) {
            Log("Found valid repository: "+dRepository.MainUri);
        }
        return(Ready);
    }

    /**
     * @return true if current DRepository is valid, otherwise false;
     */
    bool DUpdate::IsValidRepository(void) {
        if (dRepository.MainUri.empty()) {
            Log("Missing repo");
            return false;
        }

        if (dRepository.RepoType == REPO_TYPE_FOLDER) {
            if (!DTools::DPath::Exists(dRepository.MainUri)) {
                Log("Repo "+dRepository.MainUri+" does not exist");
                return false;
            }

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
        Log("Repo type "+dRepository.RepoType+" not valid");
        return false;
    }

    bool DUpdate::IsReady(void) {
        return(Ready);
    }

    bool DUpdate::IsUpdater(void) {
        return(AsUpdater);
    }

    /**
     * @brief Check for pendings operations.
     * If process filename is UPDATER_FILENAME, start files upgrade.
     * If ".justupdated" file is found this is the first start after upgrade, so, doesn't perform any update check to avoid unwanted loop (in case of error).
     */
    void DUpdate::CheckPendings(void) {
        if (AsUpdater) {
            // Run as updater
            Log("Detect run as executable updater");
            ParseLocalRepoInfoFile();

            Log("Apply update");
            if (ApplyUpdate()) {
                // Apply update and re-run main exe
                Log("Update apply sucessfully");
                std::ofstream File(fs::current_path() / FILENAME_JUST_UPDATE);
                if (File.is_open()) {
                    File.close();
                }
                else {
                    Log("Cannot create file " FILENAME_JUST_UPDATE);
                }

                std::string MainExecutable=UpdateData->ReadDotString(SECTION_UPGRADE_INFO,PARAM_MAIN_EXE,"");
                Log("Start main exe "+MainExecutable);
                if (DShell::ExecuteDetached(MainExecutable.c_str())) {
                    Log("Done");
                }
                else {
                    Log("Error");
                }
                exit(0);
            }
            else {
                Log("ERROR applying update");
            }
        }
        else if (DTools::DPath::Exists(fs::current_path() / FILENAME_JUST_UPDATE)) {
            // .justupdate file is present
            // this means that is the first run run after a succesfull update
            // so, clean all
            Log("Detected " FILENAME_JUST_UPDATE " : first time after upgrade, to avoid unwanted loop, update check is not performed.");
            err::error_code ec;
            if (!fs::remove(fs::current_path() / FILENAME_JUST_UPDATE,ec)) {
                Log("ERROR "+ec.message());
            }
            if (!fs::remove(fs::current_path() / FILENAME_UPDATER_EXE,ec)) {
                Log("ERROR "+ec.message());
            }
            Ready=false;
        }
        else if (DTools::DPath::Exists(fs::current_path() / FILENAME_UPDATER_EXE)) {
            Log("Detected " FILENAME_UPDATER_EXE " wihout " FILENAME_JUST_UPDATE " : something was wrong during last update");
            Ready=false;
        }
    }

    /**
     * @brief Execute all needed operations to Upgrade.
     */
    void DUpdate::DoUpgrade(void) {
        CheckPendings();

        if (!DownloadRemoteInfoFile()) {
            return;
        }

        if (!ParseLocalRepoInfoFile()) {
            return;
        }

        Log("Current version  "+std::to_string(CurrVersionNr));
        Log("Available version "+std::to_string(RepoVersionNr));
        if (UpdateNeeded) {
            Log("Update needed.");
        }
        else {
            Log("Update not needed.");
            return;
        }

        if (!DownloadFiles()) {
            return;
        }

        // Default updater executable is myseflf
        fs::path FromFilename=DPath::GetExeFilename();

        // Check if MainExe has been downloaded
        std::string MainExe=UpdateData->ReadDotString(SECTION_UPGRADE_INFO,PARAM_MAIN_EXE,"");
        std::vector<std::string> Files;
        std::string FilesNodeName=SECTION_FILES "." SECTION_REPLACE;
        UpdateData->ReadItemNames(FilesNodeName,Files);
        for (std::string& Source : Files) {
            std::string Dest=UpdateData->ReadDotString(FilesNodeName,Source,"");
            bool IsMainExe=false;
            #ifdef _WIN32
                IsMainExe=DString::CmpNoCase(MainExe,Dest);
            #else
                IsMainExe=MainExe == Dest;
            #endif
            if (IsMainExe) {
                // MainExe has been downloed, use it for making DuryUpdater
                FromFilename=UpdateTempDir / Source;
                break;
            }
        }

        // Generate updater
        Log("Create updater from "+FromFilename.string());
        fs::path UpdaterFilename=CurrExeName.parent_path() / FILENAME_UPDATER_EXE;
        Log("Updater executable "+UpdaterFilename.string());
        DTools::DPath::Copy_File(FromFilename,UpdaterFilename,true,true);

        Log("Run updater "+UpdaterFilename.string());
        // Run updater and exit
        bool Ret=DShell::ExecuteDetached(UpdaterFilename.string().c_str());
        if (Ret) {
            Log("Done, exit");
            exit(0);
        }
        else {
            Log("Cannot run");
        }
    }

    void DUpdate::SendFiles(std::vector<std::string> FilesList, std::string DestRepoSubPath) {
        // TODO
    }

    /**
     * @brief Download repo info file from remote repository.
     * @return true on succeed otherwise false (use GelLastStatus() to retrive error text).
     */
    bool DUpdate::DownloadRemoteInfoFile(void) {
        if (!Ready) return false;
        fs::path RemoteInfoFilename=fs::path(dRepository.MainUri) / dRepository.SubUri / FILENAME_REPO_RULES;
        if (dRepository.RepoType == REPO_TYPE_FOLDER) {
            // Copy from remote network folder
            if (dRepository.NeedAuth) {
                // TODO
            }
            std::string LogMsg="Copy repo file"+RemoteInfoFilename.string()+" to "+LocalInfoFilename.string();
            if (!DTools::DPath::Copy_File(RemoteInfoFilename,LocalInfoFilename,true,true)) {
                Log("Error "+LogMsg);
                Ready=false;
                return false;
            }
            else {
                Log("Done "+LogMsg);
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
     * @brief Parse repo info file and populate UpdateData prefs.
     * Check app name and upgrade version match.
     * @return true on succeed otherwise false (use GetLastStatus() to retrive error text).
     */
    bool DUpdate::ParseLocalRepoInfoFile(void) {
        if (!Ready) return false;

        if (UpdateData == nullptr) {
            UpdateData=new DTools::DPreferences(LocalInfoFilename.string());
            if (!UpdateData->IsReady()) {
                Log("Info file parse error: "+UpdateData->GetLastStatus());
                return false;
            }
        }

        // Check app name
        std::string RepoAppName=UpdateData->ReadString(SECTION_UPGRADE_INFO,PARAM_APP_NAME,"");
        if (RepoAppName.empty() || !DTools::DString::CmpNoCase(RepoAppName,CurrAppName)) {
            Log("AppName does not match");
            return false;
        }

        // Read Version
        std::string RepoVersionStr=UpdateData->ReadString(SECTION_UPGRADE_INFO,PARAM_UPGRADE_VERSION,"");
        RepoVersionNr=DTools::DString::ToNumber<int>(DString::RemoveNotDigit(RepoVersionStr));
        if (RepoVersionNr > CurrVersionNr) {
            UpdateNeeded=true;
        }
        else {
            UpdateNeeded=false;
        }

        return true;
    }

    /**
     * @brief Download all files needed by ugrade. Filenames are store in SECTION_REPLACE and SECTION_UPDATE section in info file.
     * @return true on succeed otherwise false (use GelLastStatus() to retrive error text).
     */
    bool DUpdate::DownloadFiles(void) {
        if (!Ready) return false;
        fs::path RemoteInfoFilename=fs::path(dRepository.MainUri) / dRepository.SubUri / FILENAME_REPO_RULES;
        if (dRepository.RepoType == REPO_TYPE_FOLDER) {
            // Copy from remote network folder
            if (dRepository.NeedAuth) {
                // TODO
            }

            // Read replace files list
            std::vector<std::string> Files;
            std::string NodeName=SECTION_FILES "." SECTION_REPLACE;
            UpdateData->ReadItemNames(NodeName,Files);
            for (std::string& Source : Files) {
                // Make filenames
                fs::path SourceFilename=fs::path(dRepository.MainUri) / dRepository.SubUri / Source;
                fs::path DestFilename=UpdateTempDir / Source; // same as repo
                if (!DTools::DPath::Exists(SourceFilename)) {
                    Log("File missing "+SourceFilename.string());
                    return false;
                }

                // copy to local temp
                std::string LogMsg="Copy "+SourceFilename.string()+" to "+DestFilename.string();
                if (!DTools::DPath::Copy_File(SourceFilename,DestFilename,false)) {
                    Log("Error "+LogMsg);
                    Ready=false;
                    return false;
                }
                else {
                    Log("Done "+LogMsg);
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
            Log("Repo type unkown????");
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
        for (std::string& Source : Files) {
            // Dest file name (real name)
            std::string RealName=UpdateData->ReadDotString(NodeName,Source,"");
            // Source filename (downloaded one)
            fs::path SourceFilename=UpdateTempDir / Source;
            if (!DTools::DPath::Exists(SourceFilename)) {
                Log("Previous downloaded file missing: "+Source);
                return false;
            }
            // Dest full filename
            fs::path DestFilename=fs::current_path() / RealName;

            // Backup if already exists
            std::string LogMsg;
            if (DTools::DPath::Exists(DestFilename)) {
                // Backup dest filename
                fs::path BakFilename=BackupDir / RealName;
                LogMsg="Backup "+DestFilename.string()+" to "+BakFilename.string();
                //err::error_code ec=DTools::DPath::Copy_File(DestFilename,BakFilename,true,true);
                err::error_code ec;
                if (DTools::DPath::Exists(BakFilename)) {
                    fs::remove(BakFilename,ec);
                }
                if (ec.value() == 0) {
                    fs::rename(DestFilename,BakFilename,ec);
                }
                if (ec.value() != 0) {
                    Log("Error "+LogMsg+" : "+ec.message());
                    Ready=false;
                    return false;
                }
                Log("Done "+LogMsg);
            }

            // Replace with new one
            LogMsg="installing file "+SourceFilename.string()+" to "+DestFilename.string();
            //err::error_code ec=DTools::DPath::Copy_File(UpdFilename,DestFilename,true);
            err::error_code ec;
            fs::rename(SourceFilename,DestFilename,ec);
            if (ec.value() != 0) {
                Log("Error "+LogMsg+" : "+ec.message());
                Ready=false;
                return false;
            }
            Log("Done "+LogMsg);
        }

        Log("Clean update folder "+UpdateTempDir.string());
        int nDeleted=DPath::DeleteFiles(UpdateTempDir,false,nullptr);
        if (nDeleted == -1) {
            Log("Cannot delete temporary udate files");
        }
        else {
            Log("Cleaned "+std::to_string(nDeleted)+" files");
        }

        return true;
    }

    // ******************************  Callback stuffs ***************************************
	/**
	 * @brief Register a class member callback.
	 * @param callback  ->  DMemberCallback type function to bind like:
	 * @code auto callback=std::bind(&MainWindow::UpdaterCallback,this,std::placeholders::_1); @endcode
	 */
	void DUpdate::SetMemberCallback(DMemberCallback callback) {
			MemberCallback=callback;
	}

	/**
	 * @brief Perform the callback.
	 * @param Msg	->	Log message to send.
	 */
	void DUpdate::DoCallback(std::string Msg) {
		if(MemberCallback) {
			MemberCallback(Msg);
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
