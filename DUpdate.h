#include <string>
#include <future>
#include "DFilesystem.h"
#include "DPreferences.h"

namespace DTools {
    class DUpdate {
        public:
            enum DRepoType {REPO_TYPE_FOLDER, REPO_TYPE_HTTP, REPO_TYPE_FTP};
            DUpdate(std::string ApplicationName, std::string CurrentVersion);
            void SetRepository(DRepoType RepoType,std::string RepoUri, std::string RepoSubUri, bool Authenticate, std::string RepoUser, std::string RepoPwd);

            // Public callback stuffs
            typedef std::function<void (std::string)> DGlobalCallback;
            typedef std::function<void (void*, std::string)> DMemberCallback;
            void SetGlobalCallback(DGlobalCallback callback);
            void SetMemberCallback(DMemberCallback callback, void *ClassObj);

        private:
            typedef struct {
                DRepoType RepoType=REPO_TYPE_FOLDER;
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
            DTools::fs::path LocalInfoFilename;
            DTools::DPreferences::DPreferences *RepoInfo;

            bool DownloadRemoteInfoFile(void);
            bool ParseRepoInfoFile(void);
            bool DownloadFiles(void)

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
