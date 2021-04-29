/**
* @file DLog.h
*
* @author $Author
*
* @version $Version
**/

#ifndef DLogH
#define DLogH

#include <sstream>
#include <memory>
#include <chrono>
#include <iomanip>
#include <iostream>


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
    *
    *
    *
    * \section futures_sec Futures
    **/
    class DLog {
        public:
            //! enum log deep level
            enum DLogLevel {PRINT_LEVEL_NORMAL, PRINT_LEVEL_DEEP};

            /**
            * @brief Contructor
            * @details Create the log object
            * @param LogFilename    ->  Filename of a destination log file, if empty no file will be crated (default empty).
            * @param StdoutEnabled  ->  If true al leg messages are output on stdout or stderr (default true).
            * @param OutputStream   ->  Additional stream for output, set nullptr to disable.
            * @param Level          ->  PRINT_LEVEL_NORMAL: Print only i() and e() call.
            *                           PRINT_LEVEL_DEEP:   Print also d() and w() call.
            **/
            DLog(std::string LogFilename = "", bool StdoutEnabled = true, std::ostream *OutputStream = nullptr, DLogLevel Level = PRINT_LEVEL_DEEP) {
                LogToStdout=StdoutEnabled;
                LogLevel=Level;
                OutStream=OutputStream;
                Filename=LogFilename;
                if (Filename == "") {
                    hFile=nullptr;
                    LogToFile=false;
                }
                else {
                    hFile=fopen(Filename.c_str(),"aw+");
                    if (hFile == nullptr) {
                        perror("Log file not opened");
                        LogToFile=false;
                        StdoutEnabled=true; // se non posso loggare su file uso comunque lo stdout
                    }
                    else {
                        LogToFile=true;
                    }
                }
            }

            //! Destructor
            ~DLog() {
                if (hFile != nullptr) fclose(hFile);
            }

            // TODO: testare
            //! Get all log content
            std::string Read(void) {
                fseek(hFile,0,SEEK_END);
                size_t fSize=ftell(hFile);
                //std::string Buff(fSize,' ');
                std::unique_ptr<char[]> buff(new char[fSize]);
                fseek(hFile,0,SEEK_SET);
                size_t len=fread(buff.get(),fSize,1,hFile);
                if (len != fSize) {
                    e("File size %d is different from readed %d bytes",fSize,len);
                }
                fseek(hFile,0,SEEK_END);

                return(std::string(buff.get()));
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

            void SetOutputStream(std::ostream *OutputStream = nullptr) {
                OutStream=OutputStream;
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

                if (OutStream == nullptr) {
                    i("Log to OutStream: OFF");
                }
                else {
                    i("Log to OutStream: ON");
                }
            }

        private:
            enum DLogOutput {OUTPUT_INFO,OUTPUT_ERROR,OUTPUT_DEBUG,OUTPUT_WARNING};
            std::string Filename;
            FILE *hFile;
            bool LogToFile;
            bool LogToStdout;
            std::ostream *OutStream;
            DLogLevel LogLevel;

            //! Colors defines for printf
            const std::string CL_RED        =   "\x1b[31m";
            const std::string CL_GREEN      =   "\x1b[32m";
            const std::string CL_YELLOW     =   "\x1b[33m";
            const std::string CL_BLUE       =   "\x1b[34m";
            const std::string CL_MAGENTA    =   "\x1b[35m";
            const std::string CL_CYAN       =   "\x1b[36m";
            const std::string CL_DEFAULT    =   "\x1b[0m";

            //! Write the the message on stdout, stderr, file
            void Write(DLogOutput Output,std::string LogMsg) {
                if (LogLevel == PRINT_LEVEL_NORMAL) {
                    if (Output > OUTPUT_ERROR) {
                        return;
                    }
                }
                auto now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::stringstream ss;
                ss << std::put_time(localtime(&now_time_t),"%Y/%m/%d %H.%M.%S : ");
                std::string HdrMsg=ss.str();
                std::string Color;
                std::string LevelMsg;

                // Debug level
                switch (Output) {
                    case OUTPUT_DEBUG:
                        LevelMsg+="DEBUG   : ";
                        Color=CL_CYAN;
                        break;
                    case OUTPUT_INFO:
                        LevelMsg+="INFO    : ";
                        Color=CL_DEFAULT;
                        break;
                    case OUTPUT_ERROR:
                        LevelMsg+="ERROR   : ";
                        Color=CL_RED;
                        break;
                    case OUTPUT_WARNING:
                        LevelMsg+="WARNING : ";
                        Color=CL_YELLOW;
                        break;
                    default:
                        break;
                }

                // Print log message
                if (LogToFile) {
                    fprintf(hFile,"%s\r",(HdrMsg+LevelMsg+LogMsg).c_str());
                }
                if (LogToStdout) {
                    printf("%s%s\n\r",(HdrMsg+Color+LevelMsg+LogMsg).c_str(),CL_DEFAULT.c_str());
                }

                if (OutStream) {
                    *OutStream << HdrMsg << LevelMsg << LogMsg << std::endl;
                }

                fflush(hFile); // Scrivi subito
            }
    };
}
#endif
