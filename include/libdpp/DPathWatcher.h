#ifndef DPathWatcherH
#define DPathWatcherH

#include "libdpp/DFilesystem.h"
#include "libdpp/DPath.h"
#include <chrono>
#include <future>

namespace DTools
{
    class DPathWatcher {
        public:
            enum DChangeStatus                                          { CHANGE_STATUS_ERROR=-1    , CHANGE_STATUS_NONE=0  , CHANGE_STATUS_CREATED=1   , CHANGE_STATUS_ERASED=2, CHANGE_STATUS_MODIFIED=3  , CHANGE_STATUS_RESUMED=4  , CHANGE_STATUS_LOG_STR , CHANGE_STATUS_WATCH_STARTED   , CHANGE_STATUS_WATCH_ENDED , CHANGE_STATUS_STOPPED_FROM_REQUEST    };
            inline static std::vector<std::string> DChangeStatusDesc=   { "Error"                   , "No changes"          , "Created"                 , "Erased"              , "Modified"                , "Resumed"                 , "Log"                 , "Watch Started"               , "Watch Ended"             , "Stopped from Request"                };
            //DPathWatcher(fs::path PathToWatch = fs::path(), std::string WatcherName = std::string(), size_t IntervalMillis = 1000);
            DPathWatcher();
            ~DPathWatcher();
            void SetInterval(size_t MSec);
            size_t GetWatchesCount(void);
            void Check(bool FireChangesOnly = false);
            bool Start(void);
            void Pause(bool Enabled = true);
            void SetStop(void);
            DError::DErrorCode SetStopAndWait(size_t TimeOutMSec = 0);
            void SetName(std::string WatcherName);
            bool IsWatching(void);
            bool IsPaused(void);
            bool IsMyPath(const fs::path& Path);
            size_t AddPath(fs::path PathToWatch, bool ScanRecursive);
            DError::DErrorCode ClearWatches(size_t TimeOutMSec);
            size_t GetWatchErrorsCount(void);

            class DPathWatch {
                public:
                    DPathWatch(fs::path PathToWatch, bool ScanRecursive);
                    DChangeStatus Check(bool& Stop);
                    DChangeStatus ScanDir(bool& Stop);
                    bool Contains(const std::string &key);
                    bool Exists(void);

                    fs::path Path;                                          //! File/dir absolute path
                    bool RecursiveScan;                                     //! Deterninate recursive scan or not
                    bool IsDirectory;                                       //! True if Path is a directory
                    bool FirstScan;                                         //! Used to avoid the first scan in constructor
                    bool LastExists;                                        //! Last exist status
                    bool Ready;
                    std::chrono::system_clock::time_point LastWriteTime;    //! File/dir last write time
                    std::chrono::system_clock::time_point LastChangeTime;   //! Last change time detected
                    DChangeStatus LastChangeStatus;                         //! Status of last change
                    std::string   LastErrorString;
                    std::unordered_map<std::string,fs::file_time_type> DirScanResult;
            };

            std::vector<DPathWatch> WatchList;
            std::string Name;
            std::string LastStrStatus;

        private:
            bool IsChanged(DPathWatch& dPathStatus);
            void Log(std::string LogMsg);
            std::string GetLastStatus(void);

            std::chrono::duration<int, std::milli> PollMilli;


            bool Watching;
            bool StopWatchingRequested;
            bool Stopping;
            bool Paused;
            size_t IntervalMSec;
            // Thread stuffs
            std::thread WatchThread;
            std::promise<bool> PromiseEnded;
            std::future<bool> ThreadFuture;

        // Callback
        public:
            typedef std::function<void (DChangeStatus, fs::path, std::string)> DPathWatcherCallback;
            void SetCallback(DPathWatcherCallback callback);
        private:
            DPathWatcherCallback Callback;
            void DoCallback(DChangeStatus ChangeStatus, const fs::path Path, const std::string Msg = std::string());
    };
}

#endif
