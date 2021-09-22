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
            enum DChangeStatus                                          { CHANGE_STATUS_ERROR=-1    , CHANGE_STATUS_NONE=0  , CHANGE_STATUS_CREATED=1   , CHANGE_STATUS_ERASED=2, CHANGE_STATUS_MODIFIED=3  , CHANGE_STATUS_LOG_STR };
            inline static std::vector<std::string> DChancgeStatusDesc=  { "Error"                   , "No changes"          , "Created"                 , "Erased"              , "Modified"                , "Log"};
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
            inline static std::string LastErrorString;

            class DPathWatch {
                public:
                    DPathWatch(fs::path PathToWatch) {
                        Path=PathToWatch;
                        FirstScan=true;
                        LastChangeStatus=CHANGE_STATUS_NONE;
                        try {
                            IsDirectory=fs::is_directory(Path);
                            LastExists=fs::exists(Path);
                            if (LastExists) {
                                LastWriteTime=DPath::LastWriteTime(Path);
                            }
                        }catch (std::exception& e) {
                            LastChangeStatus=CHANGE_STATUS_ERROR;
                            LastErrorString=e.what();
                        }
                    }

                    DChangeStatus Check(void) {
                        try {
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
                        } catch (std::exception e) {
                            LastErrorString=std::string("Check for exists ")+e.what();
                            LastChangeStatus=CHANGE_STATUS_ERROR;
                        }

                            // Check for update
                            if (LastExists) {
                            try {
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
                            } catch (std::exception e) {
                                LastErrorString=std::string(" Check for update")+e.what();
                                LastChangeStatus=CHANGE_STATUS_ERROR;
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

        // Callback
        public:
            typedef std::function<void (DChangeStatus, fs::path, std::string)> DPathWatcherCallback;
            void SetCallback(DPathWatcherCallback callback);
        private:
            DPathWatcherCallback Callback;
            void DoCallback(DChangeStatus ChangeStatus, fs::path Path, std::string Msg = std::string());
    };
}

#endif
