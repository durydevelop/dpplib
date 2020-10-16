#include "DPathWatcher.h"
#include <thread>

namespace DTools {
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
    DPathWatcher::DPathWatcher(fs::path PathToWatch) {
        GlobalCallback=nullptr;
        MemberCallback=nullptr;
        MemberCalbackObj=nullptr;
        if (!PathToWatch.empty()) {
            AddPath(PathToWatch);
        }
        else {
            Log("no watches yet");
        }
        Watching=false;
        NeedToQuit=false;
        IntervalMSec=1000;
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
     * @return true if WatchThread is running
     */
    bool DPathWatcher::IsWatching(void) {
        return Watching;
    }

    /**
     * @brief Execute a one shot check of all watches.
     * One callback is fired for each file change detected.
     * @param FireOnlyChages    ->  if true CHANGE_STATUS_NONE wil not be callbacked
     */
    void DPathWatcher::Check(bool FireOnlyChages) {
        for (DPathWatch& Watch : WatchList) {
            DChangeStatus ChangeStatus=Watch.Check();
            if (ChangeStatus == CHANGE_STATUS_NONE && FireOnlyChages) {
                continue;
            }
            DoCallback(Watch.LastChangeStatus,Watch.Path);
        }
    }

    /**
     * @brief Start a thread that performs the "one shot check" of all whatches each IntervalMSec milliseconds.
     * @return false if there are no watches in the list, otherwise true. You can call IsWatching() method to test if thread is running.
     */
    bool DPathWatcher::Start(void) {
        if (WatchList.size() == 0) {
            Log("No watch set, thread not strated");
            return false;
        }
        Log("Watch thread starting...");

        //std::promise<bool> PromiseEnded;
        ThreadFuture=PromiseEnded.get_future();

        WatchThread=std::thread([&]() {
            Log("Watch thread started");
            Watching=true;
            NeedToQuit=false;
            while (!NeedToQuit) {
                auto delta=std::chrono::steady_clock::now() + std::chrono::milliseconds(IntervalMSec);
                Check(true);
                std::this_thread::sleep_until(delta);
            }
            Watching=false;
            Log("Watch thread ended");
            PromiseEnded.set_value_at_thread_exit(true);
        });
        WatchThread.detach();
        return true;
    }

    /**
     * @brief Set stop flag to inform thread to stop watching loop. You can call IsWatching() method to test when thread is closed.
     */
    void DPathWatcher::Stop(void) {
        NeedToQuit=true;
        Log("Stop flag set");
    }

    /**
     * @brief Set stop flag to inform thread to stop watching loop and wait until thread is finished.
     * @param TimeOutMSec   ->  Nr of milliseconds to wait before return.
     * @return true if thread is really stopped, false if thread is not alive or timeout is reached.
     */
    bool DPathWatcher::StopAndWait(size_t TimeOutMSec) {
        if (!Watching) {
            Log("Watch thread is not alive, no stop needed");
            return false;
        }
        NeedToQuit=true;
        if (TimeOutMSec == 0) {
            // Default value
            TimeOutMSec=IntervalMSec*WatchList.size();
        }

        if (ThreadFuture.wait_for(std::chrono::milliseconds(TimeOutMSec)) == std::future_status::timeout) {
            Log("Watch thread stop waiting: timeout");
            return false;
        }

        Log("Watch thread stop waiting: end reached");
        return(ThreadFuture.get());
    }

    // ******************************  Callback stuffs ***************************************

    /**
     * @brief Register a global callback.
     * @param callback  ->  DGlobalCallback type function to register.
     */
    void DPathWatcher::SetGlobalCallback(DGlobalCallback callback)	{
            GlobalCallback=callback;
            MemberCallback=nullptr;
            MemberCalbackObj=nullptr;

    }

	/**
	 * @brief Register a class member callback.
	 * @param callback  ->  DMemberCallback type function to register (e.g. ClassName::CallbackFunc).
	 * @param ClassObj  ->  Class pointer in which callback is called.
	 */
	void DPathWatcher::SetMemberCallback(DMemberCallback callback, void *ClassObj) {
			GlobalCallback=nullptr;
			MemberCallback=callback;
			MemberCalbackObj=ClassObj;
	}

	/**
	 * @brief Perform the callback
	 * @param Path			->	Filename that has been changed.
	 * @param ChangeStatus	->	Detected Change type. Can be CHANGE_STATUS_CREATED, CHANGE_STATUS_ERASED, CHANGE_STATUS_MODIFIED.
	 */
	void DPathWatcher::DoCallback(DChangeStatus ChangeStatus, fs::path Path, std::string Msg) {
		if(GlobalCallback != NULL) {
			GlobalCallback(ChangeStatus,Path,Msg);
		}
		else if((MemberCallback != NULL) && (MemberCalbackObj != NULL)) {
			MemberCallback(MemberCalbackObj,ChangeStatus,Path,Msg);
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
		DoCallback(CALLBACK_STR_MSG,fs::path(),LastStrStatus);
	}

	//! @return LastStrStatus string.
	std::string DPathWatcher::GetLastStatus(void) {
		return(LastStrStatus);
	}
	// ***************************************************************************************
}
