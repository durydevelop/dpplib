#ifndef DSyncWatcherH
#define DSyncWatcherH

#include "libdpp/DFilesystem.h"
#include "libdpp/DPath.h"
#include "libdpp/DSyncFile.h"
#include <thread>
#include <chrono>
#include <future>

namespace DTools
{
    class DSyncWatcher {
        public:
            DSyncWatcher(void);
            ~DSyncWatcher();
            void SetInterval(size_t MSec);
            size_t GetSyncCount(void);
            void Check(void);
            bool Start(void);
            void Stop(void);
            bool StopAndWait(size_t TimeoutMSec = 0);
            bool Clear(size_t TimeoutMSec = 0);
            bool IsWatching(void);
            size_t AddSync(fs::path SourceFilename, fs::path DestFilename, bool SyncNow);
            void SetSafeMode(bool Enabled);
            bool GetSafeMode(void);

            std::vector<DSyncFile> SyncList;
            std::string LastStrStatus;

            // Public callback stuffs
            typedef std::function<void (DSyncFile::DSyncStatus, fs::path, std::string)> DGlobalCallback;
            typedef std::function<void (void*, DSyncFile::DSyncStatus, fs::path, std::string)> DMemberCallback;
            void SetGlobalCallback(DGlobalCallback callback);
            void SetMemberCallback(DMemberCallback callback, void *ClassObj);

        private:
            void Log(std::string LogMsg);
            std::string GetLastStatus(void);

            std::chrono::duration<int, std::milli> PollMilli;
            bool Watching;
            bool NeedToQuit;
            size_t IntervalMSec;
            bool SafeMode;
            // Thread stuffs
            std::thread WatchThread;
            std::promise<bool> PromiseEnded;
            std::future<bool> ThreadFuture;

            // Private callback stuffs
            DGlobalCallback GlobalCallback;
            DMemberCallback MemberCallback;
            void* MemberCalbackObj;
            void DoCallback(DSyncFile::DSyncStatus SyncStatus, fs::path Path, std::string Msg = std::string());
    };
}

#endif
