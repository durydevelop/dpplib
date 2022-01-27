#include "libdpp/DPathWatcher.h"
#include "libdpp/DChrono.h"
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

namespace DTools
{
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
    DPathWatcher::DPathWatcher(fs::path PathToWatch, std::string WatcherName, size_t IntervalMillis) {
        if (!PathToWatch.empty()) {
            AddPath(PathToWatch);
        }
        else {
            Log("no watches yet");
        }
        Watching=false;
        NeedToQuit=false;
        Paused=false;
        IntervalMSec=IntervalMillis;
        Name=WatcherName;
    }

    DPathWatcher::~DPathWatcher() {
        if (Watching) {
            SetStopAndWait(10000);
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
     * @return number of all watches.
     */
    size_t DPathWatcher::AddPath(fs::path PathToWatch) {
        DPathWatch dPathStatus(PathToWatch);
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
     * @brief Execute a one shot check of all watches.
     * One callback is fired for each file change detected.
     * @param FireOnlyChages    ->  if true CHANGE_STATUS_NONE wil not be callbacked.
     */
    void DPathWatcher::Check(bool FireChangesOnly) {
        for (DPathWatch& Watch : WatchList) {
            DChangeStatus ChangeStatus=Watch.Check(NeedToQuit);
            if (ChangeStatus == CHANGE_STATUS_NONE && FireChangesOnly) {
                continue;
            }
            DoCallback(Watch.LastChangeStatus,Watch.Path);
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
            NeedToQuit=false;
            Paused=false;
            DoCallback(CHANGE_STATUS_WATCH_STARTED,std::string());
            while (!NeedToQuit) {
                auto delta=std::chrono::steady_clock::now() + std::chrono::milliseconds(IntervalMSec);
                if (!Paused) {
                    Check(true);
                }
                //else {
                //    Log("paused...");
                //}
                std::this_thread::sleep_until(delta);
            }

            if (NeedToQuit) {
                Log("Stop Request");
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
        NeedToQuit=true;
        Log("Stop flag set");
    }

    /**
     * @brief Set stop flag to inform thread to stop watching loop and wait until thread is finished.
     * @param TimeOutMSec   ->  Nr of milliseconds to wait before return (default=30000).
     * @return false on timeout reached, otherwise true.
     */
    bool DPathWatcher::SetStopAndWait(size_t TimeOutMSec) {
        //Log("Stop request for "+Name);
        if (!Watching) {
            //Log("Watch thread is not alive, no stop needed");
            return true;
        }
        NeedToQuit=true;

        if (ThreadFuture.wait_for(std::chrono::milliseconds(TimeOutMSec)) == std::future_status::timeout) {
            Log("Watch thread stop waiting: timeout");
            return false;
        }

        Log("Watch thread stop waiting: normal end reached");
        return(ThreadFuture.get());
    }

    //! Clear WatchList
    void DPathWatcher::ClearWatches(size_t TimeOutMSec) {
        SetStopAndWait(TimeOutMSec);
        WatchList.clear();
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
