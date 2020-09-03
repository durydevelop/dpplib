/**
* @file DLog.h
*
* @author $Author
*
* @version $Version
**/

#ifndef DLogH
#define DLogH

#include <string>
#include <sstream>
#include <memory>
#include <chrono>
#include <iomanip>
#include <iostream>

namespace DTools {
    namespace DLog {
        /**
        * @class DLog
        *
        * @brief C++ std class for logging to stdout, stderr, file.
        *
        * @author $Author
        *
        * \mainpage DLog class
        *
        * \section intro_sec Intro
        *
        *
        *
        * \section futures_sec Futures
        **/
        class DLog
        {
            public:
            /**
            * @brief Main contructor
            * @details Create the log object
            * @param LogFilename   ->  filename of a destination log file, if empty no file will be crated (default empty).
            * @param StdoutEnabled ->  if true al leg messages are output on stdout or stderr (default true).
            **/
            DLog(std::string LogFilename = "", bool StdoutEnabled = true) {
                LogToStdout=StdoutEnabled;
                //outStream=NULL;
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

                if (LogToStdout) {
                    i("DLog to Stdout: yes");
                }
                else {
                    i("DLog to Stdout: no");
                }

                if (hFile == nullptr) {
                    i("DLog to File: no");
                }
                else {
                    i("DLog to File: &s",Filename.c_str());
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
                        e("Readind log file deceted file size %d is different from readed %d bytes",fSize,len);
                    }
                    fseek(hFile,0,SEEK_END);

                    return(std::string(buff.get()));
                }

                template<typename ... Args>
                void d(const std::string& format, Args ... args) {
                    size_t size = snprintf(nullptr,0,format.c_str(),args ...)+1;
                    if( size <= 0 ){
                        Write(DL_LEVEL_ERROR,"Log formatting failed");
                    }
                    std::unique_ptr<char[]> buf(new char[size]);
                    snprintf(buf.get(),size,format.c_str(),args ...);
                    Write(DL_LEVEL_DEBUG,buf.get());
                }

                template<typename ... Args>
                void i(const std::string& format, Args ... args)
                {
                    size_t size = snprintf(nullptr,0,format.c_str(),args ...)+1;
                    if( size <= 0 ){
                        Write(DL_LEVEL_ERROR,"Log formatting failed");
                    }
                    std::unique_ptr<char[]> buf(new char[size]);
                    snprintf(buf.get(),size,format.c_str(),args ...);
                    Write(DL_LEVEL_INFO,buf.get());
                }

                template<typename ... Args>
                void w( const std::string& format, Args ... args)
                {
                    size_t size = snprintf(nullptr,0,format.c_str(),args ...)+1;
                    if( size <= 0 ){
                        Write(DL_LEVEL_ERROR,"Log formatting failed");
                    }
                    std::unique_ptr<char[]> buf(new char[size]);
                    snprintf(buf.get(),size,format.c_str(),args ...);
                    Write(DL_LEVEL_WARNING,buf.get());
                }

                template<typename ... Args>
                void e( const std::string& format, Args ... args)
                {
                    size_t size = snprintf(nullptr,0,format.c_str(),args ...)+1;
                    if( size <= 0 ){
                        Write(DL_LEVEL_ERROR,"Log formatting failed" );
                    }
                    std::unique_ptr<char[]> buf(new char[size]);
                    snprintf(buf.get(),size,format.c_str(),args ...);
                    Write(DL_LEVEL_ERROR,buf.get());
                }

            private:
                enum DebugLevel {DL_LEVEL_DEBUG,DL_LEVEL_INFO,DL_LEVEL_WARNING,DL_LEVEL_ERROR};
                std::string Filename;
                FILE *hFile;
                bool LogToFile;
                bool LogToStdout;

                //! Colors defines for printf
                const std::string CL_RED        =   "\x1b[31m";
                const std::string CL_GREEN      =   "\x1b[32m";
                const std::string CL_YELLOW     =   "\x1b[33m";
                const std::string CL_BLUE       =   "\x1b[34m";
                const std::string CL_MAGENTA    =   "\x1b[35m";
                const std::string CL_CYAN       =   "\x1b[36m";
                const std::string CL_DEFAULT    =   "\x1b[0m";

                //! Write the the message on stdout, stderr, file
                void Write(unsigned char Level,std::string LogMsg) {
                    auto now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                    std::stringstream ss;
                    ss << std::put_time(localtime(&now_time_t),"%Y/%m/%d %H.%M.%S : ");
                    std::string HdrMsg=ss.str();
                    std::string Color;
                    std::string LevelMsg;

                    // Debug level
                    switch (Level) {
                        case DL_LEVEL_DEBUG:
                            LevelMsg+="DEBUG   : ";
                            Color=CL_MAGENTA;
                            break;
                        case DL_LEVEL_INFO:
                            LevelMsg+="INFO    : ";
                            Color=CL_DEFAULT;
                            break;
                        case DL_LEVEL_ERROR:
                            LevelMsg+="ERROR   : ";
                            Color=CL_RED;
                            break;
                        case DL_LEVEL_WARNING:
                            LevelMsg+="WARNING : ";
                            Color=CL_YELLOW;
                            break;
                        default:
                            break;
                    }

                    //Msg+=LogMsg+CL_DEFAULT+"\r\n";

                    // Print log message
                    if (LogToFile) {
                        fprintf(hFile,"%s\r",(HdrMsg+LevelMsg+LogMsg).c_str());
                    }
                    if (LogToStdout) {
                        if (Level == DL_LEVEL_ERROR)  {
                            perror((HdrMsg+Color+LevelMsg+LogMsg+CL_DEFAULT+"\r").c_str());
                        }
                        else {
                            //std::cout << HdrMsg << Color << LevelMsg << LogMsg << CL_DEFAULT << "\r";
                            printf("%s%s\n\r",(HdrMsg+Color+LevelMsg+LogMsg).c_str(),CL_DEFAULT.c_str());
                        }
                    }

                    /*
                    if (outStream != NULL) {
                        *outStream << HdrMsg << LevelMsg << LogMsg;
                    }
                    */

                    fflush(hFile); // Scrivi subito
                }
        };
    }
}
#endif
