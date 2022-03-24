#include "libdpp/DSyncWatcher.h"

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
     * @brief Constructor
     */
    DSyncWatcher::DSyncWatcher(void) {
        Callback=nullptr;
        Starting=false;
        Stopping=false;
        Watching=false;
        NeedToQuit=false;
        SafeMode=true;  // TODO: per ora (perché fs::copy_file() copia ma non copia in overwite)
        IntervalMSec=1000;
    }

    /**
     * @brief Destructor.
     * Also wait for thread to terminate.
     */
    DSyncWatcher::~DSyncWatcher() {
        Clear();
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
        DSyncFile dSyncFile(SourceFilename,DestFilename,SyncNow,SafeMode);
        if (SyncNow) {
            NotifySyncStatus(dSyncFile);
        }
        LastStrStatus=dSyncFile.LastStrStatus;
        SyncList.push_back(std::move(dSyncFile));
        return(SyncList.size());
    }

    /**
     * @brief Check if Path is one of the sync sources.
     * @param Path  ->  Pathname to search.
     * @return index of the DSyncFile containing Path as Source.
     */
    bool DSyncWatcher::IsMySource(const fs::path& Path) {
        for (size_t ixS=0; ixS<SyncList.size(); ixS++) {
            if (SyncList[ixS].Source == Path) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Check if Path is one of the sync destinations.
     * @param Path  ->  Pathname to search.
     * @return index of the DSyncFile containing Path as Dest.
     */
    bool DSyncWatcher::IsMyDest(const fs::path& Path) {
        for (size_t ixS=0; ixS<SyncList.size(); ixS++) {
            if (SyncList[ixS].Dest == Path) {
                return true;
            }
        }
        return false;
    }

    /**
     * @return number of all watches.
     */
    size_t DSyncWatcher::GetSyncCount(void) {
        return(SyncList.size());
    }

    /**
     * @brief Check if any sync are not ready.
     * @return Number of sync not ready.
     */
    size_t DSyncWatcher::GetSyncErrors(void) {
        size_t Errors=0;
        for (DSyncFile& Sync : SyncList) {
            if (!Sync.IsReady()) {
                Errors++;
            }
        }
        return (Errors);
    }

    /**
     * @brief Safe mode enable a workaround in DPath::Copy_File to bypass gcc 7 filesystem bug that doen't overwrite existing files.
     * @param Enabled   ->  If true, safe mode workaround is enabled.
     */
    void DSyncWatcher::SetSafeMode(bool Enabled) {
        SafeMode=Enabled;
        for (DSyncFile& Sync : SyncList) {
            Sync.SetSafeMode(SafeMode);
        }
    }

    /**
     * @brief Check if safe mode is enabled (see SetSafeMode()).
     * @return true if safe mode is enabled otherwise false.
     */
    bool DSyncWatcher::IsSafeMode(void) {
		return(SafeMode);
	}

    bool DSyncWatcher::Clear(int TimeoutMSec) {
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
     * @brief Execute a one-shot check of all watches.
     * Not all sync evet wil be fired over callback, only the important ones.
     */
    DSyncFile::DSyncStatus DSyncWatcher::Check(void) {
        DSyncFile::DSyncStatus SyncStatus=DSyncFile::SYNC_NO_NEEDED;
        auto itrSync=std::vector<DSyncFile>::iterator();
        for (itrSync=SyncList.begin(); itrSync != SyncList.end(); itrSync++) {
            SyncStatus=itrSync->DoSync();
            switch (SyncStatus) {
                case DTools::DSyncFile::SYNC_DONE:
                    DoCallback(CALLBACK_WATCHER_SYNC_DONE,itrSync->Source);
                    break;
                case DTools::DSyncFile::SYNC_NOT_YET:
                    break;
                case DTools::DSyncFile::SYNC_NO_NEEDED:
                    break;
                case DTools::DSyncFile::SYNC_RESTORED:
                    DoCallback(CALLBACK_WATCHER_SYNC_RESTORED,itrSync->Source);
                    break;
                case DTools::DSyncFile::SYNC_NOT_READY:
                    break;
                case DTools::DSyncFile::SYNC_CALLBACK_STR_MSG:
                    break;
                case DTools::DSyncFile::SYNC_ERR_FILE_NOT_FOUND:
                    DoCallback(CALLBACK_WATCHER_SYNC_ERROR,itrSync->Source,itrSync->LastStrStatus);
                    break;
                case DTools::DSyncFile::SYNC_ERR_SAME_AS_DEST:
                case DTools::DSyncFile::SYNC_ERR_BHO:
                case DTools::DSyncFile::SYNC_ERR_COPY:
                    DoCallback(CALLBACK_WATCHER_SYNC_ERROR,itrSync->Source,itrSync->LastStrStatus);
                    break;
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
        if (Stopping) {
            Log("Thread in stopping, cannot start");
            return false;
        }
        if (SyncList.size() == 0) {
            Log("No sync set, thread not started");
            return false;
        }

        //Log("Sync thread starting...");
        Starting=true;

        // Reset promise (for reuse)
        PromiseEnded=std::promise<bool>();
        ThreadFuture=PromiseEnded.get_future();

        WatchThread=std::thread([this]() {
            //Log("Sync thread started");
            Starting=false;
            Watching=true;
            NeedToQuit=false;
            DoCallback(CALLBACK_WATCHER_SYNC_STARTED,std::string());
            while (!NeedToQuit) {
                auto delta=std::chrono::steady_clock::now() + std::chrono::milliseconds(IntervalMSec);
                Check();
                std::this_thread::sleep_until(delta);
            }
            Watching=false;
            //Log("Sync thread ended");
            PromiseEnded.set_value_at_thread_exit(true);
            DoCallback(CALLBACK_WATCHER_SYNC_ENDED,std::string());
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
     * @return false on timeout reached, otherwise true.
     */
    bool DSyncWatcher::StopAndWait(int TimeOutMSec) {
        if (!Watching || Stopping) {
            //Log("Sync thread is not alive, no stop needed");
            return true;
        }

        NeedToQuit=true;
        Stopping=true;
        if (TimeOutMSec == TIMEOUT_INFINITE) {
            // Wait witout timeout
            ThreadFuture.wait();
        }
        else {
            // Wait with timeout
            if (TimeOutMSec == TIMEOUT_DEFAULT) {
                // Default value
                TimeOutMSec=IntervalMSec*SyncList.size();
            }
            if (ThreadFuture.wait_for(std::chrono::milliseconds(TimeOutMSec)) == std::future_status::timeout) {
                Log("Watch thread stop waiting: timeout");
                return false;
            }
        }

        Log("Watch thread stop waiting: end reached");
        bool Ret=ThreadFuture.get();
        Stopping=false;
        return(Ret);
    }

    /**
     * @brief Get the LastSyncStatus of a DSyncFile and fire a callback about it.
     * @param dSyncFile ->  DSyncFile reference to check.
     */
    void DSyncWatcher::NotifySyncStatus(DSyncFile &dSyncFile) {
        DSyncFile::DSyncStatus SyncStatus=dSyncFile.LastSyncStatus;
        switch (SyncStatus) {
            case DTools::DSyncFile::SYNC_NOT_YET:
                DoCallback(CALLBACK_WATCHER_SYNC_NOT_YET,dSyncFile.Source);
                break;
            case DTools::DSyncFile::SYNC_NO_NEEDED:
                DoCallback(CALLBACK_WATCHER_SYNC_NO_NEEDED,dSyncFile.Source);
                break;
            case DTools::DSyncFile::SYNC_DONE:
                DoCallback(CALLBACK_WATCHER_SYNC_DONE,dSyncFile.Source);
                break;
            case DTools::DSyncFile::SYNC_NOT_READY:
                DoCallback(CALLBACK_WATCHER_SYNC_NOT_READY,dSyncFile.Source);
                break;
            case DTools::DSyncFile::SYNC_RESTORED:
                DoCallback(CALLBACK_WATCHER_SYNC_RESTORED,dSyncFile.Source);
                break;
            case DTools::DSyncFile::SYNC_CALLBACK_STR_MSG:
                DoCallback(CALLBACK_WATCHER_STR_MSG,dSyncFile.Source);
                break;
            case DTools::DSyncFile::SYNC_ERR_COPY:
            case DTools::DSyncFile::SYNC_ERR_FILE_NOT_FOUND:
            case DTools::DSyncFile::SYNC_ERR_SAME_AS_DEST:
            case DTools::DSyncFile::SYNC_ERR_BHO:
                DoCallback(CALLBACK_WATCHER_SYNC_ERROR,dSyncFile.Source,dSyncFile.LastStrStatus);
                break;
        }
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
	void DSyncWatcher::DoCallback(DSyncWatcherStatus SyncStatus, fs::path Path, std::string Msg) {
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
		DoCallback(CALLBACK_WATCHER_STR_MSG,fs::path(),LastStrStatus);
	}

	//! @return LastStrStatus string.
	std::string DSyncWatcher::GetLastStatus(void) {
		return(LastStrStatus);
	}
	// ***************************************************************************************
}
