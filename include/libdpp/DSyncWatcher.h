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
            enum DSyncWatcherStatus { CALLBACK_WATCHER_SYNC_ERROR=-1, CALLBACK_WATCHER_SYNC_NOT_YET=0, CALLBACK_WATCHER_SYNC_NO_NEEDED=1, CALLBACK_WATCHER_SYNC_DONE=2, CALLBACK_WATCHER_SYNC_RESTORED=3, CALLBACK_WATCHER_SYNC_STARTED=4, CALLBACK_WATCHER_SYNC_ENDED=5, CALLBACK_WATCHER_STR_MSG=16 };
            DSyncWatcher(void);
            ~DSyncWatcher();
            void SetInterval(size_t MSec);
            size_t GetSyncCount(void);
            DSyncFile::DSyncStatus Check(void);
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

        private:
            void NotifySyncStatus(DSyncFile &dSyncFile);
            void Log(std::string LogMsg);
            std::string GetLastStatus(void);

            std::chrono::duration<int, std::milli> PollMilli;
            bool Watching;
            bool Starting;
            bool Stopping;
            bool NeedToQuit;
            size_t IntervalMSec;
            bool SafeMode;
            // Thread stuffs
            std::thread WatchThread;
            std::promise<bool> PromiseEnded;
            std::future<bool> ThreadFuture;

        // Callback
        public:
            typedef std::function<void (DSyncWatcherStatus, fs::path, std::string)> DCallback;
            void SetCallback(DCallback callback);
        private:
            DCallback Callback;
            void DoCallback(DSyncWatcherStatus SyncStatus, fs::path Path, std::string Msg = std::string());
    };
}

#endif
