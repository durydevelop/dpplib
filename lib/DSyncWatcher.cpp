#include "libdpp/DSyncWatcher.h"
//#include "libdpp/DSyncFile.h"


namespace DTools
{
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
     * @brief DSyncWatcher::DSyncWatcher
     * @param SourceFilename
     * @param DestFilename
     * @param SyncNow
     */
    DSyncWatcher::DSyncWatcher(void) {
        Callback=nullptr;
        Watching=false;
        NeedToQuit=false;
        SafeMode=false;
        IntervalMSec=1000;
    }

    /**
     * @brief Destructor. Wait 5 seconds for thread to terminate.
     */
    DSyncWatcher::~DSyncWatcher() {
        Clear(5);
    }

    /**
     * @brief Set check interval.
     * @param MSec  ->  Interval to check in mSec.
     */
    void DSyncWatcher::SetInterval(size_t MSec) {
        IntervalMSec=MSec;
    }

    /**
     * @brief DSyncWatcher::AddSync
     * @param SourceFilename
     * @param DestFilename
     * @param SyncNow
     * @return
     */
    size_t DSyncWatcher::AddSync(fs::path SourceFilename, fs::path DestFilename, bool SyncNow) {
        DSyncFile dSyncFile(SourceFilename,DestFilename,SyncNow);
        dSyncFile.SetSafeMode(SafeMode);
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

    /**
     * @brief Safe mode enable a workaround in DPath::Copy_File to bypass gcc 7 filesystem bug that doen't overwrite existing files
     * @param Enabled use SafeMode workaround
     */
    void DSyncWatcher::SetSafeMode(bool Enabled) {
        SafeMode=Enabled;
        for (DSyncFile& Sync : SyncList) {
            Sync.SetSafeMode(SafeMode);
        }
    }

	bool DSyncWatcher::GetSafeMode(void) {
		return(SafeMode);
	}

    bool DSyncWatcher::Clear(size_t TimeoutMSec) {
        if (!StopAndWait(TimeoutMSec)) {
            return false;
        }
        SyncList.clear();
        return true;
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
    void DSyncWatcher::Check(void) {
        DSyncFile::DSyncStatus SyncStatus=DSyncFile::SYNC_NO_NEEDED;
        for (DSyncFile& Sync : SyncList) {
            SyncStatus=Sync.DoSync();
            if (SyncStatus == DSyncFile::SYNC_DONE) {
                DoCallback(SyncStatus,Sync.Source);
            }
            else if (SyncStatus == DSyncFile::SYNC_ERROR) {
                DoCallback(SyncStatus,Sync.Source,Sync.LastStrStatus);
            }
        }
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
            //Log("Sync thread started");
            Watching=true;
            NeedToQuit=false;
            while (!NeedToQuit) {
                auto delta=std::chrono::steady_clock::now() + std::chrono::milliseconds(IntervalMSec);
                DSyncFile::DSyncStatus SyncStatus=DSyncFile::SYNC_NO_NEEDED;
                for (DSyncFile& Sync : SyncList) {
                    SyncStatus=Sync.DoSync();
                    if (SyncStatus == DSyncFile::SYNC_DONE) {
                        DoCallback(SyncStatus,Sync.Source);
                    }
                    else if (SyncStatus == DSyncFile::SYNC_ERROR) {
                        //DoCallback(SyncStatus,Sync.Source,Sync.LastStrStatus);
                    }
                }
                std::this_thread::sleep_until(delta);
            }
            Watching=false;
            //Log("Sync thread ended");
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
            //Log("Sync thread is not alive, no stop needed");
            return;
        }
        NeedToQuit=true;
        //Log("Stop flag set");
    }

    /**
     * @brief Set stop flag to inform thread to stop watching loop and wait until thread is finished.
     * @param TimeOutMSec   ->  Nr of milliseconds to wait before return.
     * @return true if thread is really stopped, false if thread is not alive or timeout is reached.
     */
    bool DSyncWatcher::StopAndWait(size_t TimeOutMSec) {
        if (!Watching) {
            //Log("Sync thread is not alive, no stop needed");
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
	 * @brief Register a class member callback.
	 * @param callback  ->  DCallback type function to bind like:
	 * For thread safety, callback should be set in this way:
	 * @code
	 * auto callback=std::bind(&MainWindow::Callback,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
	 * RestClient.SetCallback(callback);
	 * @endcode
	 */
	void DSyncWatcher::SetCallback(DCallback callback) {
			Callback=callback;
	}

	/**
	 * @brief Perform the callback
	 * @param Path			->	Filename that has been changed.
	 * @param SyncStatus	->	Detected Change type. Can be CHANGE_STATUS_CREATED, CHANGE_STATUS_ERASED, CHANGE_STATUS_MODIFIED.
	 */
	void DSyncWatcher::DoCallback(DSyncFile::DSyncStatus SyncStatus, fs::path Path, std::string Msg) {
		if(Callback) {
			Callback(SyncStatus,Path,Msg);
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
