#ifndef DUpdateH
#define DUpdateH

#include <string>
#include <future>
#include "libdpp/DFilesystem.h"
#include "libdpp/DPreferences.h"

namespace DTools
{
    class DUpdate {
        public:
            // Public callback stuffs
            typedef std::function<void (std::string)> DGlobalCallback;
            typedef std::function<void (void*, std::string)> DMemberCallback;

            const std::string REPO_TYPE_FOLDER= "FOLDER";
            const std::string REPO_TYPE_HTTP=   "HTTP";
            const std::string REPO_TYPE_FTP=    "FTP";

            DUpdate(const std::string ApplicationName, const std::string CurrentVersion);
            DUpdate(const std::string ApplicationName, const std::string CurrentVersion, DMemberCallback MemberCallback = nullptr, void *ClassObj = nullptr);
            DUpdate(const std::string ApplicationName, const std::string CurrentVersion, DGlobalCallback Callback = nullptr);
            ~DUpdate();

            bool SetRepositoryFromFile(DTools::fs::path Filename);
            bool SetRepository(std::string RepoType,std::string RepoUri, std::string RepoSubUri, bool Authenticate, std::string RepoUser, std::string RepoPwd);
            bool IsValidRepository(void);
            bool IsReady(void);

           // Public callback stuffs
            void SetGlobalCallback(DGlobalCallback callback);
            void SetMemberCallback(DMemberCallback callback, void *ClassObj);
            void CheckPendings(void);
            void DoUpgrade(void);

        private:
            typedef struct {
                std::string RepoType="";
                std::string MainUri;
                std::string SubUri;
                bool NeedAuth=false;
                std::string User;
                std::string Password;
            }DRepository;

            DRepository dRepository;
            std::string CurrAppName;
            int CurrVersionNr;
            int RepoVersionNr;
            bool UpdateNeeded;
            bool Ready;
            DTools::fs::path UpdateTempDir;
            DTools::fs::path BackupDir;
            DTools::fs::path LocalInfoFilename;
            DTools::DPreferences *UpdateData;
            fs::path ExeName;

            void Init(const std::string ApplicationName, const std::string CurrentVersion);
            bool DownloadRemoteInfoFile(void);
            bool ParseLocalRepoInfoFile(void);
            bool DownloadFiles(void);
            bool ApplyUpdate(void);

            // Private callback stuffs
            DGlobalCallback GlobalCallback;
            DMemberCallback MemberCallback;
            void* MemberCalbackObj;
            void DoCallback(std::string Msg = std::string());

            // Logging stuffs
            std::string LastStrStatus;
            void Log(std::string LogMsg);
            std::string GetLastStatus(void);
    };
}

#endif
