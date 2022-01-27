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
            enum DChangeStatus                                          { CHANGE_STATUS_ERROR=-1    , CHANGE_STATUS_NONE=0  , CHANGE_STATUS_CREATED=1   , CHANGE_STATUS_ERASED=2, CHANGE_STATUS_MODIFIED=3  , CHANGE_STATUS_STOPPED , CHANGE_STATUS_LOG_STR , CHANGE_STATUS_WATCH_STARTED   , CHANGE_STATUS_WATCH_ENDED };
            inline static std::vector<std::string> DChangeStatusDesc=   { "Error"                   , "No changes"          , "Created"                 , "Erased"              , "Modified"                , "Stopped"             , "Log"                 , "Watch Started"               , "Watch Ended"             };
            DPathWatcher(fs::path PathToWatch = fs::path(), std::string WatcherName = std::string(), size_t IntervalMillis = 1000);
            ~DPathWatcher();
            void SetInterval(size_t MSec);
            size_t GetWatchesCount(void);
            void Check(bool FireChangesOnly = false);
            bool Start(void);
            void Pause(bool Enabled = true);
            void SetStop(void);
            bool SetStopAndWait(size_t TimeOutMSec);
            // TODO: Kill()
            bool IsWatching(void);
            bool IsPaused(void);
            bool IsMyPath(const fs::path& Path);
            size_t AddPath(fs::path PathToWatch);
            void ClearWatches(size_t TimeOutMSec);

            class DPathWatch {
                public:
                    DPathWatch(fs::path PathToWatch) {
                        Path=PathToWatch;
                        FirstScan=true;
                        LastChangeStatus=CHANGE_STATUS_NONE;
                        try {
                            IsDirectory=DPath::IsDirectory(Path);
                            LastExists=DTools::DPath::Exists(Path);
                            if (LastExists && !IsDirectory) {
                                LastWriteTime=DPath::LastWriteTime(Path);
                            }
                        }catch (std::exception& e) {
                            LastChangeStatus=CHANGE_STATUS_ERROR;
                            LastErrorString=e.what();
                        }
                    }

                    DChangeStatus Check(bool& Stop) {
                        if (IsDirectory) {
                            if (FirstScan) {
                                // First scan
                                for (auto &File : fs::recursive_directory_iterator(Path)) {
                                    DirScanResult[File.path().string()] = fs::last_write_time(File);
                                    if (Stop) {
                                        return(CHANGE_STATUS_STOPPED);
                                    }
                                }
                                FirstScan=false;
                                return(CHANGE_STATUS_NONE);
                            }
                        }

                        try {
                            // Check for exist
                            LastChangeStatus=CHANGE_STATUS_NONE;
                            bool Exists=DTools::DPath::Exists(Path);
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
                            LastErrorString=std::string("DPpathWath check for exists exception: ")+e.what();
                            LastChangeStatus=CHANGE_STATUS_ERROR;
                        }

                        // Check for update
                        if (LastExists) {
                            try {
                                if (IsDirectory) {
                                    // Dir
                                    LastChangeStatus=ScanDir(Stop);
                                    LastChangeTime=std::chrono::system_clock::now();
                                }
                                else {
                                    // File
                                    std::chrono::system_clock::time_point NewLastWriteTime=DPath::LastWriteTime(Path);
                                    if (NewLastWriteTime != LastWriteTime) {
                                        LastChangeStatus=CHANGE_STATUS_MODIFIED;
                                        LastWriteTime=NewLastWriteTime;
                                        LastChangeTime=std::chrono::system_clock::now();
                                    }
                                }
                            } catch (std::exception e) {
                                LastErrorString=std::string("DPathWath check for update exception: ")+e.what();
                                LastChangeStatus=CHANGE_STATUS_ERROR;
                            }
                        }

                        return(LastChangeStatus);
                    }

                    DChangeStatus ScanDir(bool& Stop) {
                        // Check for files delete testing lasc scan result
                        auto it = DirScanResult.begin();
                        while (it != DirScanResult.end()) {
                            if (!DTools::DPath::Exists(it->first)) {
                                it = DirScanResult.erase(it);
                                return(CHANGE_STATUS_ERASED);
                            }
                            else {
                                it++;
                            }
                            if (Stop) {
                                return(CHANGE_STATUS_STOPPED);
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
                            if (Stop) {
                                return(CHANGE_STATUS_STOPPED);
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
                        return(DTools::DPath::Exists(Path));
                    }

                    fs::path Path;                                          //! File/dir absolute path
                    bool IsDirectory;                                       //! True if Path is a directory
                    bool FirstScan;                                         //! Used to avoid the first scan in constructor
                    bool LastExists;                                        //! Last exist status
                    std::chrono::system_clock::time_point LastWriteTime;    //! File/dir last write time
                    std::chrono::system_clock::time_point LastChangeTime;   //! Last change time detected
                    DChangeStatus LastChangeStatus;                         //! Status of last change
                    std::string   LastErrorString;
                    std::unordered_map<std::string,fs::file_time_type> DirScanResult;
            };

            std::vector<DPathWatch> WatchList;
            std::string Name;

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
            void DoCallback(DChangeStatus ChangeStatus, const fs::path Path, const std::string Msg = std::string());
    };
}

#endif
