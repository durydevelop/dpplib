#include <string>
#include <future>
#include "DFilesystem.h"
#include "DPreferences.h"

namespace DTools {
    class DUpdate {
        public:
            const std::string REPO_TYPE_FOLDER= "FOLDER";
            const std::string REPO_TYPE_HTTP=   "HTTP";
            const std::string REPO_TYPE_FTP=    "FTP";
            DUpdate(std::string ApplicationName, std::string CurrentVersion);
            ~DUpdate();
            bool SetRepositoryFromFile(DTools::fs::path Filename);
            void SetRepository(std::string RepoType,std::string RepoUri, std::string RepoSubUri, bool Authenticate, std::string RepoUser, std::string RepoPwd);
            bool IsValidRepository(void);

            // Public callback stuffs
            typedef std::function<void (std::string)> DGlobalCallback;
            typedef std::function<void (void*, std::string)> DMemberCallback;
            void SetGlobalCallback(DGlobalCallback callback);
            void SetMemberCallback(DMemberCallback callback, void *ClassObj);
            bool DoUpgrade(void);

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
            bool Ready;
            DTools::fs::path UpdateTempDir;
            DTools::fs::path BackupDir;
            DTools::fs::path LocalInfoFilename;
            DTools::DPreferences *UpdateData;

            bool DownloadRemoteInfoFile(void);
            bool ParseRepoInfoFile(void);
            bool DownloadFiles(void);
            bool ApplyUpdate(void);

            // Private callback stuffs
            DGlobalCallback GlobalCallback;
            DMemberCallback MemberCallback;
            void* MemberCalbackObj;
            void DoCallback(std::string Msg = std::string());

            // Loggging stuffs
            std::string LastStrStatus;
            void Log(std::string LogMsg);
            std::string GetLastStatus(void);
    };
}
