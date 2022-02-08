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
            typedef std::function<void(std::string)> DMemberCallback;
            void SetMemberCallback(DMemberCallback callback);
        private:
            DMemberCallback MemberCallback;
            void DoCallback(std::string Msg);

        public:
            const std::string REPO_TYPE_FOLDER= "FOLDER";
            const std::string REPO_TYPE_HTTP=   "HTTP";
            const std::string REPO_TYPE_FTP=    "FTP";

            DUpdate(const std::string ApplicationName, const std::string CurrentVersion);
            ~DUpdate();

            bool SetRepositoryFromFile(DTools::fs::path Filename);
            bool SetRepository(std::string RepoType,std::string RepoUri, std::string RepoSubUri, bool Authenticate, std::string RepoUser, std::string RepoPwd);
            bool IsValidRepository(void);
            bool IsReady(void);
            bool IsUpdater(void);
            void CheckPendings(void);
            void DoUpgrade(void);
            void SendFiles(std::vector<std::string> FilesList, std::string DestRepoSubPath);
            std::string GetLastStatus(void);
            void TestUpdater(void);

        private:
            typedef struct _DRepository{
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
            fs::path CurrExeName;
            bool AsUpdater;

            void Init(const std::string ApplicationName, const std::string CurrentVersion);
            bool DownloadRemoteInfoFile(void);
            bool ParseLocalRepoInfoFile(void);
            bool DownloadFiles(void);
            bool ApplyUpdate(bool CleanAfter = true);

            // Logging stuffs
            std::string LastStrStatus;
            void Log(std::string LogMsg);
    };
}

#endif
