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
            enum DChangeStatus                                          { CHANGE_STATUS_NONE=0  , CHANGE_STATUS_CREATED=1   , CHANGE_STATUS_ERASED=2, CHANGE_STATUS_MODIFIED=3  , CALLBACK_STR_MSG=16   };
            inline static std::vector<std::string> DChancgeStatusDesc=  { "No changes"          , "Created"                 , "Erased"              , "Modified"                , ""                    };
            DPathWatcher(fs::path PathToWatch = fs::path());
            ~DPathWatcher();
            void SetInterval(size_t MSec);
            size_t GetWatchesCount(void);
            void Check(bool FireOnlyChages = false);
            bool Start(void);
            void Pause(bool Enabled = true);
            void SetStop(void);
            bool SetStopAndWait(size_t TimeOutMSec = 0);
            // TODO: ForceStop()
            bool IsWatching(void);
            bool IsPaused(void);
            bool IsMyPath(const fs::path& Path);
            size_t AddPath(fs::path PathToWatch);

            class DPathWatch {
                public:
                    DPathWatch(fs::path PathToWatch) {
                        Path=fs::absolute(PathToWatch);
                        IsDirectory=fs::is_directory(Path);
                        FirstScan=true;
                        LastExists=fs::exists(Path);
                        if (LastExists) {
                            LastWriteTime=DPath::LastWriteTime(Path);
                        }
                    }

                    DChangeStatus Check(void) {
                        // Check for exist
                        LastChangeStatus=CHANGE_STATUS_NONE;
                        bool Exists=fs::exists(Path);
                        if (Exists != LastExists) {
                            if (Exists) {
                                LastChangeStatus=CHANGE_STATUS_CREATED;
                            }
                            else {
                                LastChangeStatus=CHANGE_STATUS_ERASED;
                            }
                            LastExists=Exists;
                            LastChangeTime=std::chrono::system_clock::now();
                        }

                        // Check for update
                        if (LastExists) {
                            std::chrono::system_clock::time_point NewLastWriteTime=DPath::LastWriteTime(Path);
                            if (IsDirectory) {
                                // Dir
                                LastChangeStatus=ScanDir();
                                LastChangeTime=std::chrono::system_clock::now();
                                if (FirstScan) {
                                    // First scan
                                    for (auto &File : fs::recursive_directory_iterator(Path)) {
                                        DirScanResult[File.path().string()] = fs::last_write_time(File);
                                    }
                                    FirstScan=false;
                                    return(CHANGE_STATUS_NONE);
                                }
                            }
                            else {
                                // File
                                if (NewLastWriteTime != LastWriteTime) {
                                    LastChangeStatus=CHANGE_STATUS_MODIFIED;
                                    LastWriteTime=NewLastWriteTime;
                                    LastChangeTime=std::chrono::system_clock::now();
                                }
                            }
                        }

                        return(LastChangeStatus);
                    }

                    DChangeStatus ScanDir(void) {
                        // Check for files delete
                        auto it = DirScanResult.begin();
                        while (it != DirScanResult.end()) {
                            if (!fs::exists(it->first)) {
                                it = DirScanResult.erase(it);
                                return(CHANGE_STATUS_ERASED);
                            }
                            else {
                                it++;
                            }
                        }

                        // Re-scan folder and check for new or modified files
                        for (auto &File : fs::recursive_directory_iterator(Path)) {
                            std::string Filename=File.path().string();
                            if (!Contains(Filename)) {
                                DirScanResult[Filename] = fs::last_write_time(File);
                                return(CHANGE_STATUS_CREATED);
                            }
                            else {
                                if (DirScanResult[Filename] != fs::last_write_time(File)) {
                                    DirScanResult[Filename] = fs::last_write_time(File);
                                    return(CHANGE_STATUS_MODIFIED);
                                }
                            }
                        }

                        return(CHANGE_STATUS_NONE);
                    }

                    // Check if DirScanResult contains a given key
                    // If your compiler supports C++20 use DirScanResult.contains(key) instead of this function
                    bool Contains(const std::string &key) {
                        auto el = DirScanResult.find(key);
                        return el != DirScanResult.end();
                    }

                    bool Exists(void) {
                        return(fs::exists(Path));
                    }

                    fs::path Path;                                          //! File/dir absolute path
                    bool IsDirectory;                                       //! True if Path is a directory
                    bool FirstScan;                                         //! Used to avoid the first scan in constructor
                    bool LastExists;                                        //! Last exist status
                    std::chrono::system_clock::time_point LastWriteTime;    //! File/dir last write time
                    std::chrono::system_clock::time_point LastChangeTime;   //! Last change time detected
                    DChangeStatus LastChangeStatus;                         //! Status of last change
                    std::unordered_map<std::string,fs::file_time_type> DirScanResult;
            };

            std::vector<DPathWatch> WatchList;

        private:
            bool IsChanged(DPathWatch& dPathStatus);
            void Log(std::string LogMsg);
            std::string GetLastStatus(void);

            std::chrono::duration<int, std::milli> PollMilli;

            bool Watching;
            bool NeedToQuit;
            bool Paused;
            size_t IntervalMSec;
            std::string LastStrStatus;
            // Thread stuffs
            std::thread WatchThread;
            std::promise<bool> PromiseEnded;
            std::future<bool> ThreadFuture;

/*
        public:
            // Public callback stuffs
            typedef std::function<void (DChangeStatus, fs::path, std::string)> DGlobalCallback;
            typedef std::function<void (void*, DChangeStatus, fs::path, std::string)> DMemberCallback;
            void SetGlobalCallback(DGlobalCallback callback);
            void SetMemberCallback(DMemberCallback callback, void *ClassObj);
        private:
            // Private callback stuffs
            DGlobalCallback GlobalCallback;
            DMemberCallback MemberCallback;
            void* MemberCalbackObj;
            void DoCallback(DChangeStatus ChangeStatus, fs::path Path, std::string Msg = std::string());
*/
        // Callback
        public:
            typedef std::function<void (DChangeStatus, fs::path, std::string)> DPathWatcherCallback;
            void SetCallback(DPathWatcherCallback callback);
        private:
            DPathWatcherCallback Callback;
            void DoNewCallback(DChangeStatus ChangeStatus, fs::path Path, std::string Msg = std::string());
    };
}

#endif
