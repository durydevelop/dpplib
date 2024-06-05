#include "dpplib/DPathWatcher.h"
#include "dpplib/DChrono.h"
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#define DEFAULT_TIMEOUT_MSEC 10000

namespace DTools
{
    /**
    * @class DPathWatch
    *
    * @brief C++ std file watcher
    *
    * @author $Author
    *
    * \mainpage DFileWatch class
    *
    * \section intro_sec Intro
    *
    *
    *
    * \section futures_sec Futures
    **/

    DPathWatcher::DPathWatch::DPathWatch(fs::path PathToWatch, bool ScanRecursive) {
        Path=PathToWatch;
        RecursiveScan=ScanRecursive;
        FirstScan=true;
        LastChangeStatus=CHANGE_STATUS_NONE;
        LastExists=DPath::Exists(Path);
        if (LastExists) {
            IsDirectory=DPath::IsDirectory(Path);
            Ready=true;
        }
        else {
            LastChangeStatus=CHANGE_STATUS_ERROR;
            LastErrorString=PathToWatch.string()+" do not exist";
            Ready=false;
        }

        if (LastExists && !IsDirectory) {
            LastWriteTime=DPath::LastWriteTime(Path);
        }
    }

    DPathWatcher::DChangeStatus DPathWatcher::DPathWatch::Check(bool& Stop) {
        if (IsDirectory && FirstScan) {
            // First scan on dir
            if (LastExists) {
                if (RecursiveScan) {
                    // With recursive iterator
                    for (auto &Entry : fs::recursive_directory_iterator(Path)) {
                        DirScanResult[Entry.path().string()] = fs::last_write_time(Entry);
                        if (Stop) {
                            return(CHANGE_STATUS_STOPPED_FROM_REQUEST);
                        }
                    }
                }
                else {
                    // Without recursive iterator
                    for (auto &Entry : fs::directory_iterator(Path)) {
                        DirScanResult[Entry.path().string()] = fs::last_write_time(Entry);
                        if (Stop) {
                            return(CHANGE_STATUS_STOPPED_FROM_REQUEST);
                        }
                    }
                }
                FirstScan=false;
            }
            return(CHANGE_STATUS_NONE);
        }

        // Check main entry for exists
        LastChangeStatus=CHANGE_STATUS_NONE;
        bool Exists=DPath::Exists(Path);
        if (Exists != LastExists) {
            if (Exists) {
                // From !Exists to Exists
                LastChangeStatus=CHANGE_STATUS_RESUMED;
                LastErrorString.clear();
            }
            else {
                LastChangeStatus=CHANGE_STATUS_ERROR;
                LastErrorString=Path.string()+" dos not exist";
            }
            LastExists=Exists;
            Ready=Exists;
            LastChangeTime=std::chrono::system_clock::now();
            return(LastChangeStatus);
        }

        // Check for update
        if (Exists) {
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
            } catch (std::exception &e) {
                LastErrorString=std::string("DPathWath check for update exception: ")+e.what();
                LastChangeStatus=CHANGE_STATUS_ERROR;
            }
        }

        return(LastChangeStatus);
    }

    DPathWatcher::DChangeStatus DPathWatcher::DPathWatch::ScanDir(bool& Stop) {
        // Check for files delete testing lasc scan result
        auto it = DirScanResult.begin();
        while (it != DirScanResult.end()) {
            if (!DPath::Exists(it->first)) {
                it = DirScanResult.erase(it);
                return(CHANGE_STATUS_ERASED);
            }
            else {
                it++;
            }
            if (Stop) {
                return(CHANGE_STATUS_STOPPED_FROM_REQUEST);
            }
        }

        // Re-scan folder and check for new or modified files
        if (RecursiveScan) {
            // With recursive iterator
            for (auto &Entry : fs::recursive_directory_iterator(Path)) {
                std::string Filename=Entry.path().string();
                if (!Contains(Filename)) {
                    DirScanResult[Filename] = fs::last_write_time(Entry);
                    return(CHANGE_STATUS_CREATED);
                }
                else {
                    if (DirScanResult[Filename] != fs::last_write_time(Entry)) {
                        DirScanResult[Filename] = fs::last_write_time(Entry);
                        return(CHANGE_STATUS_MODIFIED);
                    }
                }
                if (Stop) {
                    return(CHANGE_STATUS_STOPPED_FROM_REQUEST);
                }
            }
        }
        else {
            // Without recursive iterator
            for (auto &Entry : fs::directory_iterator(Path)) {
                std::string Filename=Entry.path().string();
                if (!Contains(Filename)) {
                    DirScanResult[Filename] = fs::last_write_time(Entry);
                    return(CHANGE_STATUS_CREATED);
                }
                else {
                    if (DirScanResult[Filename] != fs::last_write_time(Entry)) {
                        DirScanResult[Filename] = fs::last_write_time(Entry);
                        return(CHANGE_STATUS_MODIFIED);
                    }
                }
                if (Stop) {
                    return(CHANGE_STATUS_STOPPED_FROM_REQUEST);
                }
            }
        }

        return(CHANGE_STATUS_NONE);
    }

    // Check if DirScanResult contains a given key
    // If your compiler supports C++20 use DirScanResult.contains(key) instead of this function
    bool DPathWatcher::DPathWatch::Contains(const std::string &key) {
        auto el = DirScanResult.find(key);
        return el != DirScanResult.end();
    }

    bool DPathWatcher::DPathWatch::Exists(void) {
        return(DPath::Exists(Path));
    }

    /**
    * @class DPathWatcher
    *
    * @brief C++ std file watcher
    *
    * @author $Author
    *
    * \mainpage DFileWatcher class
    *
    * \section intro_sec Intro
    *
    *
    *
    * \section futures_sec Futures
    **/

    /**
     * @brief Constructor.
     * @param PathToWatch   ->  Filename or Dir to add to WatchList. If no Path is specified, watch list is empty and it is possible to add path with AddPath() method.
     */
/*  // TODO: Se il costruttore contiene già il path non più possibile aggiungere
    DPathWatcher::DPathWatcher(fs::path PathToWatch, std::string WatcherName, size_t IntervalMillis) {
        if (!PathToWatch.empty()) {
            AddPath(PathToWatch);
        }
        else {
            Log("no watches yet");
        }
        Watching=false;
        StopWatchingRequested=false;
        Stopping=false;
        Paused=false;
        IntervalMSec=IntervalMillis;
        Name=WatcherName;
    }
*/

    DPathWatcher::DPathWatcher() {
        Watching=false;
        StopWatchingRequested=false;
        Stopping=false;
        Paused=false;
        IntervalMSec=1000;
        Name="";
    }

    DPathWatcher::~DPathWatcher() {
        if (Watching) {
            SetStopAndWait();
        }
    }

    /**
     * @brief Set check interval.
     * @param MSec  ->  Interval to check in mSec.
     */
    void DPathWatcher::SetInterval(size_t MSec) {
        IntervalMSec=MSec;
    }

    /**
     * @brief Add path to watch list
     * @param PathToWatch   ->  Filename or Dir to add to WatchList.
     * @param Recursive     ->  Applied only on folders: if false, PathToWatch folder is scanned without recursion, so, only files in this
     *                          folder are really checked, directories are cheked only for date time change (that does't fire files content changes in it).
     * @return number of all watches.
     */
    size_t DPathWatcher::AddPath(fs::path PathToWatch, bool ScanRecursive) {
        DPathWatch dPathStatus(PathToWatch,ScanRecursive);
        WatchList.push_back(dPathStatus);
        Log(std::to_string(WatchList.size()) + " tot watches");
        return(WatchList.size());
    }

    /**
     * @return number of all watches.
     */
    size_t DPathWatcher::GetWatchesCount(void) {
        return(WatchList.size());
    }

    /**
     * @return true if WatchThread is running.
     */
    bool DPathWatcher::IsWatching(void) {
        return(Watching);
    }

    /**
     * @return true if watcher is paused.
     */
    bool DPathWatcher::IsPaused(void) {
        return(Paused);
    }

    bool DPathWatcher::IsMyPath(const fs::path& Path) {
        for (size_t ixW=0; ixW<WatchList.size(); ixW++) {
            if (WatchList[ixW].Path == Path) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Check if any watches are not ready.
     * @return Number of watches not ready.
     */
    size_t DPathWatcher::GetWatchErrorsCount(void) {
        size_t Errors=0;
        for (DPathWatch& Watch : WatchList) {
            if (!Watch.Ready) {
                Errors++;
            }
        }
        return (Errors);
    }

    /**
     * @brief Set Watcher Name.
     * @param WatcherName   ->  Name you want to give it.
     */
    void DPathWatcher::SetName(std::string WatcherName) {
        Name=WatcherName;
    }

    /**
     * @brief Execute a one shot check of all watches.
     * One callback is fired for each file change detected.
     * @param FireOnlyChages    ->  if true CHANGE_STATUS_NONE wil not be callbacked.
     */
    void DPathWatcher::Check(bool FireChangesOnly) {
        for (DPathWatch& Watch : WatchList) {
            DChangeStatus ChangeStatus=Watch.Check(StopWatchingRequested);
            if (ChangeStatus == CHANGE_STATUS_NONE && FireChangesOnly) {
                continue;
            }
            DoCallback(Watch.LastChangeStatus,Watch.Path);
        }

        if (StopWatchingRequested) {
            //StopWatchingRequested=false;
            Log("Stop Request");
        }
    }

    /**
     * @brief Start a thread that performs the "one shot check" of all whatches each IntervalMSec milliseconds.
     * @return false if there are no watches in the list, otherwise true. You can call IsWatching() method to test if thread is running.
     * N.B. Interval time starts only after Check() function returns.
     */
    bool DPathWatcher::Start(void) {
        if (Watching) {
            return true;
        }
        if (Stopping) {
            Log("Thread in stopping, cannot start");
            return false;
        }
        if (WatchList.size() == 0) {
            Log("No watch set, thread not strated");
            return false;
        }
        //og("Watch thread starting...");

        // Reset promise (for reuse)
        PromiseEnded=std::promise<bool>();
        ThreadFuture=PromiseEnded.get_future();

        WatchThread=std::thread([this]() {
            //Log("Watch thread started");
            Watching=true;
            StopWatchingRequested=false;
            Paused=false;
            DoCallback(CHANGE_STATUS_WATCH_STARTED,std::string());
            while (!StopWatchingRequested) {
                auto delta=std::chrono::steady_clock::now() + std::chrono::milliseconds(IntervalMSec);
                if (!Paused) {
                    Check(true);
                }
                //else {
                //    Log("paused...");
                //}
                std::this_thread::sleep_until(delta);
            }

/*
            auto LastMillis=DChrono::NowMillis();
            while (!NeedToQuit) {
                if ((DChrono::NowMillis() - LastMillis) > IntervalMSec) {
                    if (!Paused) {
                        Check(true);
                    }
                    else {
                        Log("paused...");
                    }
                    LastMillis=DChrono::NowMillis();
                }
                std::this_thread::sleep_for(500ms);
            }
*/
            Watching=false;
            //Log("Watch thread "+Name+" ended");
            PromiseEnded.set_value_at_thread_exit(true);
            DoCallback(CHANGE_STATUS_WATCH_ENDED,std::string());
        });
        WatchThread.detach();
        return true;
    }

    /**
     * @brief Pause or un-pause the Watcher.
     * If thread is running, still active but Check() is not called.
     * @param Enabled   ->  true pause the watcher, false un-pouse.
     */
    void DPathWatcher::Pause(bool Enabled) {
        Paused=Enabled;
    }

    /**
     * @brief Set stop flag to inform thread to stop watching loop. You can call IsWatching() method to test when thread is closed.
     */
    void DPathWatcher::SetStop(void) {
        if (!Watching) {
            return;
        }
        StopWatchingRequested=true;
        Log("Stop flag set");
    }

    /**
     * @brief Set stop flag to inform thread to stop watching loop and wait until thread is finished.
     * @param TimeOutMSec   ->  Nr of milliseconds to wait before return (default=30000).
     * @return false on timeout reached, otherwise true.
     */
    DError::DErrorCode DPathWatcher::SetStopAndWait(size_t TimeOutMSec) {
        DError::DErrorCode ErrorCode;
        //Log("Stop request for "+Name);
        if (!Watching || Stopping) {
            //Log("Watch thread is not alive, no stop needed");
            return(ErrorCode);
        }

        if (TimeOutMSec == 0) {
            // TODO infinite?
            TimeOutMSec=DEFAULT_TIMEOUT_MSEC;
        }
        StopWatchingRequested=true;
        Stopping=true;

        if (ThreadFuture.wait_for(std::chrono::milliseconds(TimeOutMSec)) == std::future_status::timeout) {
            //Log("Watch thread stop waiting: timeout");
            ErrorCode.SetError("TIMEOUT");
            return(ErrorCode);
        }

        //Log("Watch thread stop waiting: normal end reached");
        bool Ret=ThreadFuture.get();
        if (!Ret) {
            ErrorCode.SetError("ThreadFuture.get() false");
        }
        Stopping=false;
        return(ErrorCode);
    }

    //! Clear WatchList
    DError::DErrorCode DPathWatcher::ClearWatches(size_t TimeOutMSec) {
        DError::DErrorCode ErrorCode=SetStopAndWait(TimeOutMSec);
        if (!ErrorCode.IsSet()) {
            WatchList.clear();
        }
        return(ErrorCode);
    }

    // ******************************  Callback stuffs ***************************************
    /**
     * @brief Register a class member callback.
     * @param callback  ->  DCallback type function to bind like:
     * @code auto callback=std::bind(&MainWindow::Callback,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3); @endcode
     */
    void DPathWatcher::SetCallback(DPathWatcherCallback callback) {
            Callback=callback;
    }

	/**
	 * @brief Perform the callback
	 */
	void DPathWatcher::DoCallback(DChangeStatus ChangeStatus, const fs::path Path, const std::string Msg) {
		if(Callback) {
			Callback(ChangeStatus,Path,Msg);
		}
	}
	// ***************************************************************************************

	// *******************************  Log Functions ****************************************
	/**
	* @brief Set LastStrStatus and make log callback.
	* @param LogMsg	->  Message to log.
	*
	* N.B. If LogMsg is empty, callback with LastStrStatus is performed.
	**/
	void DPathWatcher::Log(std::string LogMsg) {
		if (!LogMsg.empty()) {
			LastStrStatus=LogMsg;
		}
		DoCallback(CHANGE_STATUS_LOG_STR,Name,LastStrStatus);
	}

	//! @return LastStrStatus string.
	std::string DPathWatcher::GetLastStatus(void) {
		return(LastStrStatus);
	}
	// ***************************************************************************************
}
