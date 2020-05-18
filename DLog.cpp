#include "DLog.h"
#include <cstdarg>
#include <memory>
#include <iostream>
#include <cstdio>
#include <chrono>
#include <iomanip>

//! Colors defines for printf
#define CL_RED     "\x1b[31m"
#define CL_GREEN   "\x1b[32m"
#define CL_YELLOW  "\x1b[33m"
#define CL_BLUE    "\x1b[34m"
#define CL_MAGENTA "\x1b[35m"
#define CL_CYAN    "\x1b[36m"
#define CL_DEFAULT "\x1b[0m"

//! Debug levels defines
#define DL_LEVEL_DEBUG      0
#define DL_LEVEL_INFO       1
#define DL_LEVEL_WARNING    2
#define DL_LEVEL_ERROR      3

namespace DTools {
    namespace DLog {
        /*! \mainpage DLog class
        *
        * \section intro_sec Intro
        *
        *
        *
        * \section futures_sec Futures
        **/

        // TODO ParsedPrint()
        /*
            for (int ixP=0;ixP < len; ixP++) {
                if (isprint(buff[ixP])) putchar(buff[ixP]);
                else putchar('.');
            }
            */

        //! Contructor
        /**
        * Create the log object
        * @param LogFilename   ->  filename of a destination log file, if empty no file will be crated (default empty).
        * @param StdoutEnabled ->  if true al leg messages are output on stdout or stderr (default true).
        **/
        DLog::DLog(std::string LogFilename, bool StdoutEnabled) {
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

            i("DLog -> Stdout = %s , Filename = %s",LogToStdout ? "Yes" : "No",Filename.empty() ? "No file" : Filename.c_str());
        }

        //! Destructor
        DLog::~DLog() {
            if (hFile != nullptr) fclose(hFile);
        }

        /*
        void DLog::SetAdditionalStream(ostringstream *outputStream)
        {
            outStream=outputStream;
        }
        */

        std::string DLog::Read(void) {
            fseek(hFile,0,SEEK_END);
            long int fSize=ftell(hFile);
            std::string Buff(fSize,' ');
            fseek(hFile,0,SEEK_SET);
            fread((char*)Buff.c_str(),fSize,1,hFile);
            fseek(hFile,0,SEEK_END);

            return(Buff);
        }

        void DLog::d(const std::string fmt,...) {
            va_list args;
            va_start(args,fmt);
            size_t len=vsnprintf(nullptr,0,fmt.c_str(),args);
            std::unique_ptr<char[]> msg(new char[len+1]);
            vsprintf(msg.get(),fmt.c_str(),args);
            Write(DL_LEVEL_DEBUG,msg.get());
            va_end(args);

        }

        void DLog::i(const std::string fmt,...) {
            va_list args;
            va_start(args,fmt);
            size_t len=vsnprintf(nullptr,0,fmt.c_str(),args);
            std::unique_ptr<char[]> msg(new char[len+1]);
            vsprintf(msg.get(),fmt.c_str(),args);
            Write(DL_LEVEL_INFO,msg.get());
            va_end(args);

        }

        void DLog::e(const std::string fmt,...) {
            va_list args;
            va_start(args,fmt);
            size_t len=vsnprintf(nullptr,0,fmt.c_str(),args);
            std::unique_ptr<char[]> msg(new char[len+1]);
            vsprintf(msg.get(),fmt.c_str(),args);
            Write(DL_LEVEL_ERROR,msg.get());
            va_end(args);

        }

        void DLog::w(const std::string fmt,...) {
            va_list args;
            va_start(args,fmt);
            size_t len=vsnprintf(nullptr,0,fmt.c_str(),args);
            std::unique_ptr<char[]> msg(new char[len+1]);
            vsprintf(msg.get(),fmt.c_str(),args);
            Write(DL_LEVEL_WARNING,msg.get());
            va_end(args);

        }

        void DLog::Write(unsigned char Level,std::string LogMsg) {
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
                fprintf(hFile,(HdrMsg+LevelMsg+LogMsg+"\r").c_str());
            }
            if (LogToStdout) {
                if (Level == DL_LEVEL_ERROR)  {
                    perror((HdrMsg+Color+LevelMsg+LogMsg+CL_DEFAULT+"\r").c_str());
                }
                else {
                    //cout << HdrMsg << Color << LevelMsg << LogMsg << CL_DEFAULT << "\r";
                    printf((HdrMsg+Color+LevelMsg+LogMsg+CL_DEFAULT+"\r").c_str());
                }
            }

            /*
            if (outStream != NULL) {
                *outStream << HdrMsg << LevelMsg << LogMsg;
            }
            */

            fflush(hFile); // Scrivi subito
        }
    }
}
