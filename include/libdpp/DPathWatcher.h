#ifndef DPathWatcherH
#define DPathWatcherH

#include "DFilesystem.h"
#include "DPath.h"
#include <chrono>
#include <future>

namespace DTools {
    class DPathWatcher {
        public:
            enum DChangeStatus {CHANGE_STATUS_NONE=0, CHANGE_STATUS_CREATED=1, CHANGE_STATUS_ERASED=2, CHANGE_STATUS_MODIFIED=3, CALLBACK_STR_MSG=16};
            DPathWatcher(fs::path PathToWatch = fs::path());
            void SetInterval(size_t MSec);
            size_t GetWatchesCount(void);
            void Check(bool FireOnlyChages = false);
            bool Start(void);
            void Stop(void);
            bool StopAndWait(size_t TimeOutMSec);
            bool IsWatching(void);
            size_t AddPath(fs::path PathToWatch);

            // Public callback stuffs
            typedef std::function<void (DChangeStatus, fs::path, std::string)> DGlobalCallback;
            typedef std::function<void (void*, DChangeStatus, fs::path, std::string)> DMemberCallback;
            void SetGlobalCallback(DGlobalCallback callback);
            void SetMemberCallback(DMemberCallback callback, void *ClassObj);

            class DPathWatch {
                public:
                    DPathWatch(fs::path PathToWatch) {
                        Path=fs::absolute(PathToWatch);
                        LastExists=fs::exists(Path);
                        IsDirectory=fs::is_directory(Path);
                        if (LastExists) {
                            if (IsDirectory) {
                                // Dir
                                // TODO: ScanDir
                            }
                            else {
                                // File
                                LastWriteTime=DPath::LastWriteTime(Path);
                            }
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
                            if (IsDirectory) {
                                // Dir
                                // TODO: ScanDir
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
                        }

                        return(LastChangeStatus);
                    }

                    bool Exists(void) {
                        return(fs::exists(Path));
                    }

                    fs::path Path;                                          //! File/dir absolute path
                    bool IsDirectory;
                    bool LastExists;                                        //! Last exist status
                    std::chrono::system_clock::time_point LastWriteTime;    //! File/dir last write time
                    std::chrono::system_clock::time_point LastChangeTime;   //! Last change time detected
                    DChangeStatus LastChangeStatus;                         //! Status of last change
            };

        private:
            bool IsChanged(DPathWatch& dPathStatus);
            void Log(std::string LogMsg);
            std::string GetLastStatus(void);

            std::chrono::duration<int, std::milli> PollMilli;
            std::vector<DPathWatch> WatchList;

            bool Watching;
            bool NeedToQuit;
            size_t IntervalMSec;
            std::string LastStrStatus;
            // Thread stuffs
            std::thread WatchThread;
            std::promise<bool> PromiseEnded;
            std::future<bool> ThreadFuture;

            // Private callback stuffs
            DGlobalCallback GlobalCallback;
            DMemberCallback MemberCallback;
            void* MemberCalbackObj;
            void DoCallback(DChangeStatus ChangeStatus, fs::path Path, std::string Msg = std::string());
    };
}

#endif
