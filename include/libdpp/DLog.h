#ifndef DLogH
#define DLogH

#include <sstream>
#include <memory>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <functional>
#include <set>
#include <regex>

namespace DTools
{
    /**
    * @class DLog
    *
    * @brief C++ std class for easy logging to stdout, stderr, file, stream.
    *
    * @author $Author
    *
    * \mainpage DLog class
    * To enable colors in console on Windows run this command:
    * reg add HKEY_CURRENT_USER\Console /v VirtualTerminalLevel /t REG_DWORD /d 0x00000001 /f
    * To disable:
    * reg add HKEY_CURRENT_USER\Console /v VirtualTerminalLevel /t REG_DWORD /d 0x00000000 /f
    *
    * \section intro_sec Intro
    * This file can be used as single header lib, simply @code#include "DLog.h"@code
    * Log format for file:
    * DateTime              :   Message Type    :   Message Text
    *
    * Log format for stdout:
    * Color pattern : DateTime            : Message Type : Message Text    : Color Restore
    * \x1b[31m      : 2022/02/03 20.59.17 : DEBUG        : reading file... : \x1b[0m
    *
    *
    * \section futures_sec Futures
    * -2 deep levels.
    * -Storage files can be set for be kept by max size or by time period.
    * -Errors are shown on stderr.
    *
    * \section todo_sec TODO
    * _Read(FromWhen,SinceWhen);
    **/

    namespace fs=std::filesystem;

    class DLog {
        public:
            //! Log deep levels
            enum DLogLevel {PRINT_LEVEL_NORMAL, PRINT_LEVEL_DEEP};
            //! Output types
            enum DOutputType {OUTPUT_INFO, OUTPUT_ERROR, OUTPUT_DEBUG, OUTPUT_WARNING};
            //! Storage modes
            enum DStorageMode {STORAGE_MODE_SIZE, STORAGE_MODE_TIME};
            //! Date Time format string
            const std::string TIME_FORMAT="%Y/%m/%d %H.%M.%S";
            //! Separator pattern
            std::string Sep=" : ";

            bool LogToFile;                 //! if true messages are written to file.
            bool LogToStdout;               //! if true messages are written to stdout.

            /**
            * @brief Contructor
            * @details Create the log object
            * @param LogFilename    ->  Filename of a destination log file, if empty no file will be crated (default empty).
            * @param StdoutEnabled  ->  If true al leg messages are output on stdout or stderr (default true).
            * @param OutputStream   ->  Additional stream for output, set nullptr to disable.
            * @param Level          ->  PRINT_LEVEL_NORMAL: Print only i() and e() call.
            *                           PRINT_LEVEL_DEEP:   Print also d() and w() call.
            **/
            DLog(std::string LogFilename = "", bool StdoutEnabled = true, DLogLevel Level = PRINT_LEVEL_DEEP) {
                LogToStdout=StdoutEnabled;
                LogLevel=Level;
                Callback=nullptr;
                Filename=LogFilename;
                hFile=nullptr;
                LogToFile=false;
                LogToStdout=StdoutEnabled;
                CurrFPos=0;
                // Default storage mode
                // Size mode, single file size 10 MB, total storage size 1 GB
                SetStorageMode(STORAGE_MODE_SIZE,10,1000);

                // Create parent dir if not exists
                fs::path LogDir=fs::path(Filename).parent_path();
                std::error_code ec;
                fs::file_status Status=fs::status(LogDir,ec);
                if (!fs::exists(Status)) {
                    fs::create_directories(LogDir,ec);
                    if (ec) {
                        perror(("Log file <"+Filename+"> ERROR: "+ec.message()).c_str());
                        return;
                    }
                }

                OpenLogFile();
            }

            //! Destructor
            ~DLog() {
                if (hFile != nullptr) fclose(hFile);
            }

            bool Read() {
                std::ifstream fLog(Filename,std::ios::in);
                if (!fLog.is_open()) {
                    Write(OUTPUT_ERROR,"Log file not opened for read request");
                    return false;
                }
                std::string Line;
                while(std::getline(fLog,Line)) {
                    if (Line.empty()) {
                        continue;
                    }

                    std::string Header(Line.substr(0,Line.find(Sep)));
                    std::string OutputType(Line.substr(Header.size()+Sep.size(),7));
                    std::string Msg(Line.substr(Header.size()+Sep.size()+OutputType.size()+Sep.size()));
                    DoCallback(Msg,OutputType,Header);
                }
                fLog.close();
                return true;
            }

            template<typename ... Args>
            void d(const std::string& formatStr, Args ... args) {
                size_t size = snprintf(nullptr,0,formatStr.c_str(),args ...)+1;
                if( size <= 0 ){
                    Write(OUTPUT_ERROR,"Log formatting failed");
                }
                std::unique_ptr<char[]> buf(new char[size]);
                snprintf(buf.get(),size,formatStr.c_str(),args ...);
                Write(OUTPUT_DEBUG,buf.get());
            }

            void d(const std::string& LogStr) {
                Write(OUTPUT_DEBUG,LogStr);
            }

            template<typename ... Args>
            void i(const std::string& formatStr, Args ... args)
            {
                size_t size = snprintf(nullptr,0,formatStr.c_str(),args ...)+1;
                if( size <= 0 ){
                    Write(OUTPUT_ERROR,"Log formatting failed");
                }
                std::unique_ptr<char[]> buf(new char[size]);
                snprintf(buf.get(),size,formatStr.c_str(),args ...);
                Write(OUTPUT_INFO,buf.get());
            }

            void i(const std::string& LogStr) {
                Write(OUTPUT_INFO,LogStr);
            }

            template<typename ... Args>
            void w( const std::string& formatStr, Args ... args)
            {
                size_t size = snprintf(nullptr,0,formatStr.c_str(),args ...)+1;
                if( size <= 0 ){
                    Write(OUTPUT_ERROR,"Log formatting failed");
                }
                std::unique_ptr<char[]> buf(new char[size]);
                snprintf(buf.get(),size,formatStr.c_str(),args ...);
                Write(OUTPUT_WARNING,buf.get());
            }

            void w(const std::string& LogStr) {
                Write(OUTPUT_WARNING,LogStr);
            }

            template<typename ... Args>
            void e( const std::string& formatStr, Args ... args)
            {
                size_t size = snprintf(nullptr,0,formatStr.c_str(),args ...)+1;
                if( size <= 0 ){
                    Write(OUTPUT_ERROR,"Log formatting failed" );
                }
                std::unique_ptr<char[]> buf(new char[size]);
                snprintf(buf.get(),size,formatStr.c_str(),args ...);
                Write(OUTPUT_ERROR,buf.get());
            }

            void e(const std::string& LogStr) {
                Write(OUTPUT_ERROR,LogStr);
            }

            void SetLogLevel(DLogLevel Level) {
                LogLevel=Level;
            }

            DLogLevel GetLogLevel(void) {
                return(LogLevel);
            }

            void SetVerbose(bool Enabled) {
                LogToStdout=Enabled;
            }

            std::string GetFilename(void) {
                return(Filename);
            }

            //! Log current settings as info
            void PrintStatus(void) {
                if (LogToStdout) {
                    i("Log to Stdout: ON");
                }
                else {
                    i("Log to Stdout: OFF");
                }

                if (hFile == nullptr) {
                    i("Log to File: OFF");
                }
                else {
                    i("Log to File: "+Filename);
                }

                if (Callback) {
                    i("Log Callback: ON");
                }
                else {
                    i("Log Callback: OFF");
                }
            }

            /**
             * @brief Set files storage mode and parameters.
             * 2 modes are avalilable:
             *      -STORAGE_MODE_SIZE : when storage size exceed max size, oldest file is deleted.
             *      -STORAGE_MODE_TIME : when oldest file in storage is exceed max numbers of days, is deleted.
             * @param Mode          ->  One of DStorageMode enum.
             *                          In STORAGE_MODE_SIZE ModeParam must be entire storage max size in MB.
             *                          In STORAGE_MODE_TIME ModeParam must be max number of days to keep storage files.
             * @param FileSizeMB    ->  Max file size in MB of each log file.
             * @param ModeParam     ->  Total storage size in MB for STORAGE_MODE_SIZE, nubmer of days to keep for STORAGE_MODE_TIME.
             */
            void SetStorageMode(DStorageMode Mode, size_t FileSizeMB, size_t ModeParam) {
                StorageMode=Mode;
                // MB to B
                StorageFileSize=FileSizeMB*1024*1024;
                if (Mode == STORAGE_MODE_SIZE) {
                    StorageModeParam=ModeParam*1024*1024;
                }
                else if (Mode == STORAGE_MODE_TIME) {
                    StorageModeParam=ModeParam;
                }
            }

            /**
             * @brief Delete oldest file that exceed storage policy.
             */
            void DeleteOldestFile(void) {
                std::set<fs::path> FilesList;
                std::string LogExt=fs::path(Filename).extension().string();
                std::string LogName=fs::path(Filename).stem().string();
                fs::path LogDir=fs::path(Filename).parent_path();
                for (fs::directory_iterator itr(LogDir); itr != fs::directory_iterator(); ++itr) {
                    if (!fs::is_regular_file(itr->status())) {
                        continue;
                    }
                    std::string CurrName=itr->path().stem().string();
                    std::string CurrExt=itr->path().extension().string();
                    // TODO cmp no case
                    if (CurrExt == LogExt) {
                        // ext match
                        if (CurrName.find(LogName) != std::string::npos) {
                            // name match
                            if (CurrName.size() > LogName.size()) {
                                // is not current one
                                FilesList.insert(*itr);
                            }
                        }
                    }
                }

                if (FilesList.empty()) {
                    return;
                }

                if (StorageMode == STORAGE_MODE_SIZE) {
                    // calculate storage size
                    size_t TotSize=0;
                    for (auto itr=FilesList.rbegin(); itr != FilesList.rend(); itr++) {
                        TotSize+=fs::file_size(*itr);
                        if (TotSize > StorageModeParam) {
                            std::error_code ec;
                            fs::remove(*itr,ec);
                            if (ec) {
                                std::string Err="DLogFile error deleting <"+(*itr).string()+"> "+ec.message();
                                perror(Err.c_str());
                            }
                        }
                    }
                }
                else if (StorageMode == STORAGE_MODE_TIME) {
                    // find delete all files exceed max days
                    for (auto& File : FilesList) {
                        // get now as time_point
                        std::chrono::system_clock::time_point now=std::chrono::system_clock::now();
                        // get file_time of file
                        fs::file_time_type fttime=fs::last_write_time(File);
                        // convert to time_t
                        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(fttime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
                        time_t ftimet=std::chrono::system_clock::to_time_t(sctp);
                        // then in time_point
                        std::chrono::system_clock::time_point tptime=std::chrono::system_clock::from_time_t(ftimet);
                        // and make the difference as hours
                        std::chrono::hours diff=std::chrono::duration_cast<std::chrono::hours>(now - tptime);

                        if (diff.count() > StorageModeParam*24) {
                            std::error_code ec;
                            fs::remove(File,ec);
                            if (ec) {
                                std::string Err="DLogFile error deleting <"+File.string()+"> "+ec.message();
                                perror(Err.c_str());
                            }
                        }
                    }
                }
            }

            /**
             * @brief Check storage files for:
             * -Deleting old files due to the storage policy.
             * -Creating new file if current exceed size.
             */
            void CheckStorage(void) {
                if (CurrFPos > StorageFileSize) {
                    // Current log file side exceeded
                    // Delete old ones
                    DeleteOldestFile();

                    // Rename current file
                    if(!RenameFileAsNow()) {
                        return;
                    }

                    // Create new one
                    if (!OpenLogFile()) {
                        return;
                    }
                }
            }

            /**
             * @brief Open current log file, if doesn't exists, create a new one.
             * @return true on success otherwise false.
             */
            bool OpenLogFile(void) {
                // Open or create file
                hFile=fopen(Filename.c_str(),"aw+");
                if (hFile == nullptr) {
                    std::string Err="DLogFile <"+Filename+"> not opened";
                    perror(Err.c_str());
                    LogToFile=false;
                    LogToStdout=true; // se non posso loggare su file uso comunque lo stdout
                    return false;
                }

                LogToFile=true;
                // seek to eof
                fseek(hFile,0,SEEK_END);
                fgetpos(hFile,&CurrFPos);
                return true;
            }

            /**
             * @brief Rename current log file as "Name_YYYY_MM_DD_HH_MM_SS.ext"
             * @return
             */
            bool RenameFileAsNow(void) {
                if (hFile != nullptr) {
                    fclose(hFile);
                    hFile=nullptr;
                }

                std::string LogExt=fs::path(Filename).extension().string();
                std::string LogName=fs::path(Filename).stem().string();
                fs::path LogDir=fs::path(Filename).parent_path();
                // get now() formatted string
                auto now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::stringstream ss;
                ss << std::put_time(localtime(&now_time_t),"_%Y_%m_%d_%H_%M_%S");
                // create new name
                fs::path NewName=LogDir / (LogName+ss.str()+LogExt);
                std::error_code ec;
                // rename it
                fs::rename(Filename,NewName,ec);
                if (ec) {
                    std::string Err="DLogFile <"+Filename+"> "+ec.message();
                    perror(Err.c_str());
                    return false;
                }
                return true;
            }

        private:
            std::string Filename;
            FILE *hFile;
            DLogLevel LogLevel;
            DStorageMode StorageMode;
            fpos_t CurrFPos;
            fpos_t StorageFileSize;
            size_t StorageModeParam;

            //! Colors defines for printf
            const std::string CL_RED        =   "\x1b[31m";
            const std::string CL_GREEN      =   "\x1b[32m";
            const std::string CL_YELLOW     =   "\x1b[33m";
            const std::string CL_BLUE       =   "\x1b[34m";
            const std::string CL_MAGENTA    =   "\x1b[35m";
            const std::string CL_CYAN       =   "\x1b[36m";
            const std::string CL_DEFAULT    =   "\x1b[0m";

        // ******************************  Callback stuffs ***************************************
        public:
            typedef std::function<void(std::string,std::string,std::string)> DLogCallback;
            /**
             * @brief Register a class member callback.
             * @param callback  ->  DCallback type function to bind like:
             * @code auto callback=std::bind(&MainWindow::Callback,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3); @endcode
             */
            void SetCallback(DLogCallback callback = nullptr) {
                    Callback=callback;
            }
        private:
            DLogCallback Callback;
            /**
             * @brief Perform the callback
             */
            void DoCallback(std::string Msg, std::string OutputType, std::string Header) {
                Callback(Msg,OutputType,Header);
            }
        // ***************************************************************************************

        private:
            //! Write the the message on stdout, stderr, file
            void Write(DOutputType OutputType,std::string LogMsg) {
                if (LogLevel == PRINT_LEVEL_NORMAL) {
                    if (OutputType > OUTPUT_ERROR) {
                        return;
                    }
                }
                auto now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::stringstream ss;
                ss << std::put_time(localtime(&now_time_t),TIME_FORMAT.c_str());
                std::string HdrMsg=ss.str();
                std::string Color;
                std::string LevelMsg;

                // Debug level
                switch (OutputType) {
                    case OUTPUT_DEBUG:
                        LevelMsg+="DEBUG  ";
                        Color=CL_CYAN;
                        break;
                    case OUTPUT_INFO:
                        LevelMsg+="INFO   ";
                        Color=CL_DEFAULT;
                        break;
                    case OUTPUT_ERROR:
                        LevelMsg+="ERROR  ";
                        Color=CL_RED;
                        break;
                    case OUTPUT_WARNING:
                        LevelMsg+="WARNING";
                        Color=CL_YELLOW;
                        break;
                    default:
                        break;
                }

                // Print log message
                if (LogToFile) {
                    fprintf(hFile,"%s\n",(HdrMsg+Sep+LevelMsg+Sep+LogMsg).c_str());
                }
                if (LogToStdout) {
                    printf("%s%s\n\r",(Color+HdrMsg+Sep+LevelMsg+Sep+LogMsg).c_str(),CL_DEFAULT.c_str());
                }
                if (Callback) {
                    DoCallback(LogMsg,LevelMsg,HdrMsg);
                }

                // Write imediatly
                fflush(hFile);

                // Update file pos
                fgetpos(hFile,&CurrFPos);
                CheckStorage();
            }
    };
}
#endif
