#include "DSyncWatcher.h"
#include "DSyncFile.h"


namespace DTools {
    /**
    * @class DSyncWatcher
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
    DSyncWatcher::DSyncWatcher(fs::path SourceFilename, fs::path DestFilename, bool SyncNow) {
        GlobalCallback=nullptr;
        MemberCallback=nullptr;
        MemberCalbackObj=nullptr;
        if (!SourceFilename.empty() && !DestFilename.empty()) {
            AddSync(SourceFilename,DestFilename,SyncNow);
        }
        else {
            Log("no watches yet");
        }
        Watching=false;
        NeedToQuit=false;
        SafeMode=false;
        IntervalMSec=1000;
    }

    /**
     * @brief Set check interval.
     * @param MSec  ->  Interval to check in mSec.
     */
    void DSyncWatcher::SetInterval(size_t MSec) {
        IntervalMSec=MSec;
    }

    /**
     * @brief Add path to watch list
     * @param PathToWatch   ->  Filename or Dir to add to WatchList.
     * @return number of all watches.
     */
    size_t DSyncWatcher::AddSync(fs::path SourceFilename, fs::path DestFilename, bool SyncNow) {
        DSyncFile dSyncFile(SourceFilename,DestFilename,SyncNow);
        Log(dSyncFile.LastStrStatus);
        SyncList.push_back(dSyncFile);
        return(SyncList.size());
    }

    /**
     * @return number of all watches.
     */
    size_t DSyncWatcher::GetSyncCount(void) {
        return(SyncList.size());
    }

    void DSyncWatcher::SetSafeMode(bool Enabled) {
        SafeMode=Enabled;
        for (DSyncFile& Sync : SyncList) {
            Sync.SetSafeMode(SafeMode);
        }
    }

	bool DSyncWatcher::GetSafeMode(void) {
		return(SafeMode);
	}

    /**
     * @return true if WatchThread is running
     */
    bool DSyncWatcher::IsWatching(void) {
        return Watching;
    }

    /**
     * @brief Execute a one shot check of all watches.
     * One callback is fired for each file change detected.
     */
    DSyncFile::DSyncStatus DSyncWatcher::Check(void) {
        DSyncFile::DSyncStatus SyncStatus=DSyncFile::SYNC_NO_NEEDED;
        for (DSyncFile& Sync : SyncList) {
            SyncStatus=Sync.DoSync();
            if (SyncStatus == DSyncFile::SYNC_DONE) {
                DoCallback(SyncStatus,Sync.Source);
            }
            else if (SyncStatus == DSyncFile::SYNC_ERROR) {
                DoCallback(SyncStatus,Sync.Source);
            }
        }
        return(SyncStatus);
    }

    /**
     * @brief Start a thread that performs the "one shot check" of all whatches each IntervalMSec milliseconds.
     * @return false if there are no watches in the list, otherwise true. You can call IsWatching() method to test if thread is running.
     */
    bool DSyncWatcher::Start(void) {
        if (Watching) {
            Log("Already started");
            return false;
        }
        if (SyncList.size() == 0) {
            Log("No sync set, thread not started");
            return false;
        }
        Log("Sync thread starting...");

        //std::promise<bool> PromiseEnded;
        ThreadFuture=PromiseEnded.get_future();

        WatchThread=std::thread([&]() {
            Log("Sync thread started");
            Watching=true;
            NeedToQuit=false;
            while (!NeedToQuit) {
                auto delta=std::chrono::steady_clock::now() + std::chrono::milliseconds(IntervalMSec);
                Check();
                std::this_thread::sleep_until(delta);
            }
            Watching=false;
            Log("Sync thread ended");
            PromiseEnded.set_value_at_thread_exit(true);
        });

        WatchThread.detach();
        return true;
    }

    /**
     * @brief Set stop flag to inform thread to stop watching loop. You can call IsWatching() method to test when thread is closed.
     */
    void DSyncWatcher::Stop(void) {
        if (!Watching) {
            Log("Sync thread is not alive, no stop needed");
            return;
        }
        NeedToQuit=true;
        Log("Stop flag set");
    }

    /**
     * @brief Set stop flag to inform thread to stop watching loop and wait until thread is finished.
     * @param TimeOutMSec   ->  Nr of milliseconds to wait before return.
     * @return true if thread is really stopped, false if thread is not alive or timeout is reached.
     */
    bool DSyncWatcher::StopAndWait(size_t TimeOutMSec) {
        if (!Watching) {
            Log("Sync thread is not alive, no stop needed");
            return false;
        }
        NeedToQuit=true;
        if (TimeOutMSec == 0) {
            // Default value
            TimeOutMSec=IntervalMSec*SyncList.size();
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
    void DSyncWatcher::SetGlobalCallback(DGlobalCallback callback)	{
            GlobalCallback=callback;
            MemberCallback=nullptr;
            MemberCalbackObj=nullptr;
    }

	/**
	 * @brief Register a class member callback.
	 * @param callback  ->  DMemberCallback type function to register (e.g. ClassName::CallbackFunc).
	 * @param ClassObj  ->  Class pointer in which callback is called.
	 */
	void DSyncWatcher::SetMemberCallback(DMemberCallback callback, void *ClassObj) {
			GlobalCallback=nullptr;
			MemberCallback=callback;
			MemberCalbackObj=ClassObj;
	}

	/**
	 * @brief Perform the callback
	 * @param Path			->	Filename that has been changed.
	 * @param SyncStatus	->	Detected Change type. Can be CHANGE_STATUS_CREATED, CHANGE_STATUS_ERASED, CHANGE_STATUS_MODIFIED.
	 */
	void DSyncWatcher::DoCallback(DSyncFile::DSyncStatus SyncStatus, fs::path Path, std::string Msg) {
		if(GlobalCallback != NULL) {
			GlobalCallback(SyncStatus,Path,Msg);
		}
		else if((MemberCallback != NULL) && (MemberCalbackObj != NULL)) {
			MemberCallback(MemberCalbackObj,SyncStatus,Path,Msg);
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
	void DSyncWatcher::Log(std::string LogMsg) {
		if (!LogMsg.empty()) {
			LastStrStatus=LogMsg;
		}
		DoCallback(DSyncFile::CALLBACK_STR_MSG,fs::path(),LastStrStatus);
	}

	//! @return LastStrStatus string.
	std::string DSyncWatcher::GetLastStatus(void) {
		return(LastStrStatus);
	}
	// ***************************************************************************************
}
